#include <algorithm>
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <atomic>
#include <errno.h>
#include <fcntl.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <iostream>
#include <map>
#include <memory>
#include <netinet/in.h>
#include <pthread.h>
#include <queue>
#include <sched.h>
#include <shared_mutex>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <system_error>
#include <type_traits>
#include <unistd.h>
#include <vector>

#include "rpc.pb.h"

#define MAX_EVENTS 1024
#define EVENT_TIMEOUT 10

#define mlog_info(x) std::cout << x << std::endl;
#define mlog_error(x) std::cerr << x << std::endl;

static struct sockaddr_in server;
static int sock = -1;
static int epfd = -1;

struct hodor {
    int client;
    std::string msg;
};

void recv_run(int fd);

void setnonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
}

void dowork(int sig, siginfo_t *siginfo, void *data) {
    struct hodor *dor = NULL;
    switch (sig) {
    case SIGUSR1:
        dor = (struct hodor *)siginfo->si_value.sival_ptr;
        std::cout << "recv sig: " << sig << ", fd: " << dor->client
                  << ", msg: " << dor->msg.c_str() << std::endl;
        free(dor);
        dor = NULL;
        break;
    case SIGUSR2:
        recv_run(siginfo->si_value.sival_int);
        break;
    }
}

void register_signal(int signo) {
    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_sigaction = dowork;
    act.sa_flags = SA_SIGINFO;

    if (sigaction(signo, &act, NULL) < 0) {
        std::cerr << "install signal failed" << std::endl;
    }
}

void signal_action(int fd, int signo) {
    union sigval myval;

    myval.sival_int = fd;
    sigqueue(getpid(), signo, myval);
}

#define MAX_CONN_BUF 1024
#define MAGIC "peng"

enum {
    M_MAGIC = 1,
    M_HEADER = 2,
    M_RPC_HEADER = 3,
    M_RPC_CONTENT = 4,
};

struct MLengthHeader {
    int rpc_header_length;
    int rpc_body_length;
};

class ReadBuf {
public:
    ReadBuf(): _capacity(1024), _cur_index(0), _remain_size(1024) {}
    ~ReadBuf() {}

    void Next(char** buf, int* size) {
        int usable_size = std::min(*size, _remain_size);
        
        *buf = _buf + _cur_index;
        *size = usable_size;
        
        _remain_size -= usable_size;
        _cur_index += usable_size;
    }

    int32_t Capacity() const {
        return _capacity;
    }

    int32_t Usable() const {
        return _remain_size;
    }

    void Reset() {
        _cur_index = 0;
        _remain_size = 1024;
    }

private:
    char _buf[1024];
    int32_t _capacity;

    int32_t _cur_index;
    int32_t _remain_size;
};

class WriteBuf {
    public:
        WriteBuf(): _capacity(1024){

        }
        ~WriteBuf() {}

    private:
        char _buf[1024];
        int _capacity;
};

struct MConn {
    int fd;
    char buf[MAX_CONN_BUF];
    int recv_offset;
    int recv_remain;
    std::string client_addr;
    long client_port;
    int parse_type;
    int parse_remain;
    int parse_offset;

    int magic;
    struct MLengthHeader length_header;
    char *rpc_header;
    char *rpc_body;
};

class mConnManager {
  public:
    void CreateConn(int fd, std::string &client_addr, long client_port) {
        if (_conns.find(fd) != _conns.end()) {
            std::cout << "client fd exit ,cant accept again" << std::endl;
            return;
        }
        std::shared_ptr<MConn> conn(new MConn);
        conn->fd = fd;
        conn->client_addr = client_addr;
        conn->client_port = client_port;
        conn->recv_offset = 0;
        conn->recv_remain = MAX_CONN_BUF;
        conn->parse_type = M_MAGIC;
        conn->parse_remain = 4;
        conn->parse_offset = 0;
        conn->rpc_header = nullptr;
        conn->rpc_body = nullptr;
        _conns[fd] = conn;
    }

    std::shared_ptr<MConn> GetConn(int fd) {
        auto it = _conns.find(fd);
        if (it == _conns.end()) {
            return nullptr;
        }
        return it->second;
    }

    void CloseConn(int fd) {
        
        struct epoll_event ev;
        ev.data.fd = fd;
        epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
        
        auto it = _conns.find(fd);
        if (it != _conns.end()) {
           _conns.erase(fd);
        }
        
        close(fd);
    }
  private:
    std::map<int, std::shared_ptr<MConn>> _conns;
};

std::shared_ptr<mConnManager> gConnManager(new mConnManager);

class MRequest {
  public:
    MRequest(char *header, char *body, int hlength, int blength,
             std::shared_ptr<MConn> conn)
        : header_length(hlength), body_length(blength), rpc_header(header),
          rpc_body(body), _conn(conn) {}
    ~MRequest() {}

    std::shared_ptr<MConn> GetConn() { return _conn; }

  public:
    int header_length;
    int body_length;
    char *rpc_header;
    char *rpc_body;
    std::shared_ptr<MConn> _conn;
};

std::queue<std::shared_ptr<MRequest>>
parse_connection(std::shared_ptr<MConn> conn, int length) {
    int remain = 0;
    std::queue<std::shared_ptr<MRequest>> reqs;
    while (length > 0) {
        switch (conn->parse_type) {
        case M_MAGIC:
            remain = std::min(conn->parse_remain, length);
            memcpy(reinterpret_cast<char *>(&conn->magic) + 4 -
                       conn->parse_remain,
                   conn->buf + conn->parse_offset, remain);
            conn->parse_remain -= remain;
            conn->parse_offset += remain;
            length -= remain;

            if (conn->parse_remain > 0)
                break;
            conn->parse_type = M_HEADER;
            conn->parse_remain = sizeof(MLengthHeader);
            break;
        case M_HEADER:
            remain = std::min(conn->parse_remain, length);
            memcpy(reinterpret_cast<char *>(&conn->length_header) +
                       sizeof(MLengthHeader) - conn->parse_remain,
                   conn->buf + conn->parse_offset, remain);
            conn->parse_remain -= remain;
            conn->parse_offset += remain;
            length -= remain;

            if (conn->parse_remain > 0)
                break;
            conn->parse_type = M_RPC_HEADER;
            conn->parse_remain = conn->length_header.rpc_header_length;
            conn->rpc_header = new char[conn->length_header.rpc_header_length];
            conn->rpc_body = new char[conn->length_header.rpc_body_length];

            break;
        case M_RPC_HEADER:
            remain = std::min(conn->parse_remain, length);
            memcpy(conn->rpc_header + conn->length_header.rpc_header_length -
                       conn->parse_remain,
                   conn->buf + conn->parse_offset, remain);
            conn->parse_remain -= remain;
            conn->parse_offset += remain;
            length -= remain;

            if (conn->parse_remain > 0)
                break;
            conn->parse_type = M_RPC_CONTENT;
            conn->parse_remain = conn->length_header.rpc_body_length;
            break;
        case M_RPC_CONTENT:
            remain = std::min(conn->parse_remain, length);
            memcpy(conn->rpc_body + conn->length_header.rpc_body_length -
                       conn->parse_remain,
                   conn->buf + conn->parse_offset, remain);
            conn->parse_remain -= remain;
            conn->parse_offset += remain;
            length -= remain;

            if (conn->parse_remain > 0)
                break;
            reqs.push(std::make_shared<MRequest>(
                conn->rpc_header, conn->rpc_body,
                conn->length_header.rpc_header_length,
                conn->length_header.rpc_body_length, conn));
            conn->parse_type = M_MAGIC;
            conn->parse_remain = 4;
            break;
        default:
            break;
        }
    }
    return reqs;
}

class HelloTask {
  public:
    HelloTask(std::shared_ptr<M::HelloReq> req,
              std::shared_ptr<M::HelloRes> res)
        : _req(req), _res(res) {}
    ~HelloTask() {}

    void Run() { _res->set_name("world"); }

  private:
    std::shared_ptr<M::HelloReq> _req;
    std::shared_ptr<M::HelloRes> _res;
};

void do_response(std::shared_ptr<MRequest> req, M::RpcHeader &rpc_header,
                 std::shared_ptr<M::HelloRes> hellores) {
    int remain = 4 + sizeof(MLengthHeader) + rpc_header.ByteSizeLong() +
                 hellores->ByteSizeLong();
    char buf[1024];
    MLengthHeader mlh;
    mlh.rpc_header_length = int(rpc_header.ByteSizeLong());
    mlh.rpc_body_length = int(hellores->ByteSizeLong());
    memcpy(buf, MAGIC, 4);
    memcpy(buf + 4, &mlh, sizeof(MLengthHeader));
    rpc_header.SerializeToArray(buf + 4 + sizeof(MLengthHeader),
                                rpc_header.ByteSizeLong());
    hellores->SerializeToArray(buf + 4 + sizeof(MLengthHeader) +
                                   rpc_header.ByteSizeLong(),
                               hellores->ByteSizeLong());
    while (remain > 0) {
        int ret = send(req->GetConn()->fd, buf, remain, 0);
        if (ret == 0) {
            mlog_info("conn close");
            gConnManager->CloseConn(req->GetConn()->fd);
            return;
        } else if (ret < 0) {
            switch (errno) {
            case EAGAIN | EWOULDBLOCK:
                break;
            case EINTR:
                break;
            default:
                gConnManager->CloseConn(req->GetConn()->fd);
                return;
            }
        } else {
            remain -= ret;
            *buf += ret;
        }
    }
}
void do_request(std::queue<std::shared_ptr<MRequest>> reqs) {
    while (!reqs.empty()) {
        auto it = reqs.front();
        M::RpcHeader rpc_header;
        rpc_header.ParseFromArray(it->rpc_header, it->header_length);
        reqs.pop();

        if (rpc_header.func() == "hello") {
            std::shared_ptr<M::HelloReq> req = std::make_shared<M::HelloReq>();
            std::shared_ptr<M::HelloRes> res = std::make_shared<M::HelloRes>();
            req->ParseFromArray(it->rpc_body, it->body_length);
            mlog_info("req: " << req->SerializeAsString());
            HelloTask task(req, res);
            task.Run();

            rpc_header.set_type(M::RPC_TYPE_RPC_RESPONSE);
            do_response(it, rpc_header, res);
        }
    }
}

void recv_run(int fd) {
    std::shared_ptr<MConn> conn = gConnManager->GetConn(fd);
    if (nullptr == conn) {
        mlog_error("conntion read data, but connection handler is not in "
                   "connection set");
        close(fd);
        return;
    }
    do {
        int len = recv(fd, conn->buf + conn->recv_offset, conn->recv_remain, 0);
        if (len < 0) {
            switch (errno) {
            case EAGAIN | EWOULDBLOCK:
                return;
            case EINTR:
                break;
            default:
                gConnManager->CloseConn(fd); 
                return;
            }
        } else if (len > 0) {
            conn->recv_remain = conn->recv_remain - len;
            conn->recv_offset = conn->recv_offset + len;
            auto reqs = parse_connection(conn, len);
            if (!reqs.empty()) {
                do_request(reqs);
            }
        } else {
            gConnManager->CloseConn(fd); 
        }
    } while (true);
}

void accept_run() {
    epfd = epoll_create(MAX_EVENTS);
    if (epfd < 0) {
        std::cout << "create epoll failed" << std::endl;
        return;
    }

    struct epoll_event event, ev;
    event.data.fd = sock;
    event.events = EPOLLIN | EPOLLOUT;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event) == -1) {
        std::cout << "epoll add failed." << std::endl;
        close(epfd);
        return;
    }

    struct epoll_event events[MAX_EVENTS];
    struct sockaddr_in client_addr;
    socklen_t cliaddr_len = sizeof(client_addr);
    std::string client_ip;
    long client_port;

    std::cout << "start to wait connect." << std::endl;
    while (true) {
        int fdset = epoll_wait(epfd, events, MAX_EVENTS, EVENT_TIMEOUT);
        for (int i = 0; i < fdset; ++i) {
            if (events[i].data.fd == sock) {
                int client =
                    accept(sock, (struct sockaddr *)&client_addr, &cliaddr_len);
                if (client < 0) {
                    std::cerr << "accept failed. errno: " << errno << std::endl;
                    continue;
                }
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client;
                setnonblock(client);
                client_ip = inet_ntoa(client_addr.sin_addr);
                client_port = ntohl(client_addr.sin_port);
                mlog_info("client: " << client_ip << ":" << client_port
                                     << " connected");

                if (epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev) < 0) {
                    std::cerr << "epoll add client fd failed." << std::endl;
                    close(epfd);
                    return;
                }
                gConnManager->CreateConn(client, client_ip, client_port);
            } else {
                if(events[i].events & EPOLLIN) {
                    recv_run(events[i].data.fd);
                }
                if(events[i].events & EPOLLOUT){
                    //TODO 
                }
                // signal_action(events[i].data.fd, SIGUSR2);
            }
        }
    }
}

static void daemon() {
    pid_t pid;
    pid = fork();
    if (pid < 0)
        exit(-1);
    if (pid > 0)
        exit(0);
}

static void listen_port(const int port, int &sock) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cout << "create socket failed." << std::endl;
        return;
    }

    bzero(&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        std::cout << "bind failed." << std::endl;
        close(sock);
        return;
    }

    if (listen(sock, MAX_EVENTS) < 0) {
        std::cout << "listen failed." << std::endl;
        close(sock);
        return;
    }
}

static void set_multi_env() {
    struct rlimit limit;

    limit.rlim_cur = limit.rlim_max = 1024;
    if (setrlimit(RLIMIT_STACK, &limit) != 0) {
        std::cout << "set system stack size failed." << std::endl;
        exit(0);
    }

    limit.rlim_cur = limit.rlim_max = 1000000;
    if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
        std::cout << "set system open file failed." << std::endl;
        exit(0);
    }

    getrlimit(RLIMIT_STACK, &limit);
    if (limit.rlim_cur != 1024) {
        std::cout << "set system stack size failed." << std::endl;
        exit(0);
    }

    getrlimit(RLIMIT_NOFILE, &limit);
    if (limit.rlim_cur != 1000000) {
        std::cout << "set system open file failed." << std::endl;
        exit(0);
    }
}

static void watch_dog() {
    int cpunum = sysconf(_SC_NPROCESSORS_CONF);
    std::map<int, int> pool;

    std::queue<int> myqueue;
    for (int i = 0; i < cpunum; ++i)
        myqueue.push(i);

    cpu_set_t mask;
    while (true) {
        while (myqueue.size() > 0) {
            CPU_ZERO(&mask);
            CPU_SET(myqueue.front(), &mask);

            int pid = fork();
            if (pid == 0) {
                if (sched_setaffinity(0, sizeof(mask), &mask) < 0)
                    std::cerr << "sched_setaffinity failed." << std::endl;
                return;
            } else if (pid < 0) {
                exit(1);
            }

            pool[myqueue.front()] = pid;
            myqueue.pop();
        }
        for (auto &its : pool) {
            int ret = waitpid(its.second, NULL, WNOHANG);
            if (ret == its.second) {
                pool[its.first] = 0;
                myqueue.push(its.first);
            }
        }
        sleep(1);
    }
}

int main(int argc, char *argv[]) {
    // daemon();
    set_multi_env();

    register_signal(SIGPIPE);
    register_signal(SIGILL);

    listen_port(8090, sock);

    // watch_dog();

    accept_run();

    close(sock);
    return 0;
}
