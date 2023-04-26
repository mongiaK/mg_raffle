#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <fcntl.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <shared_mutex>
#include <string>
#include <system_error>
#include <type_traits>
#include <vector>

#include "buffer_manager.h"
#include "connection.h"
#include "connection_manager.h"
#include "hello_rpc_task.h"
#include "parser.h"
#include "request.h"
#include "rpc.pb.h"
#include "util.h"

#define MAX_EVENTS 1024
#define EVENT_TIMEOUT 10

static struct sockaddr_in server;
static int sock = -1;
static int epfd = -1;

struct hodor {
    int client;
    std::string msg;
};

void recv_run(int fd);

MBufferPoolSP gbufpool(new MBufferPool);

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
            mlog_info("recv sig: " << sig << ", fd: " << dor->client
                                   << ", msg: " << dor->msg.c_str());
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
        mlog_error("install signal failed");
    }
}

void signal_action(int fd, int signo) {
    union sigval myval;

    myval.sival_int = fd;
    sigqueue(getpid(), signo, myval);
}

MConnManagerSP gConnManager(new MConnManager(epfd));

std::queue<MRequestSP> parse_connection(MConnectionSP conn, MBufferSP buf,
                                        int length) {
    int remain = 0;
    std::queue<std::shared_ptr<MRequest>> reqs;
    while (length > 0) {
        switch (conn->ParseType()) {
            case M_MAGIC:
                remain = std::min(conn->MagicRemain(), length);
                memcpy(reinterpret_cast<char *>(conn->Magic()) + 4 -
                           conn->MagicRemain(),
                       buf->Buf(), remain);
                buf->Consume(remain);
                conn->MagicRemain(remain);
                length -= remain;

                if (conn->MagicRemain() > 0) break;
                conn->ChangeParseType(M_HEADER);
                break;
            default:
                if (!conn->HasParser()) {
                    conn->ResetParser(std::make_shared<MParser>(conn));
                }

                MRequestSP req = conn->Parser()->SplitMessage(buf, length);
                if (req != nullptr) {
                    reqs.push(req);
                }
                break;
        }
    }
    return reqs;
}

void do_response(MRequestSP req, M::RpcHeader &rpc_header,
                 std::shared_ptr<M::HelloRes> hellores) {
    int remain = 4 + sizeof(MLengthHeader) + rpc_header.ByteSizeLong() +
                 hellores->ByteSizeLong();
    MBufferSP bufsp;
    bufsp = gbufpool->Allocate();
    if (bufsp == nullptr) {
        mlog_error("cannot allocate buf from pool, close connection");
        return;
    }
    char* buf = bufsp->Buf();

    MLengthHeader mlh;
    mlh.rpc_header_length = int(rpc_header.ByteSizeLong());
    mlh.rpc_body_length = int(hellores->ByteSizeLong());
    memcpy(buf, MAGIC, 4);
    memcpy(buf + 4, &mlh, sizeof(MLengthHeader));
    rpc_header.SerializeToArray(buf + 4 + sizeof(MLengthHeader),
                                rpc_header.ByteSizeLong());
    hellores->SerializeToArray(
        buf + 4 + sizeof(MLengthHeader) + rpc_header.ByteSizeLong(),
        hellores->ByteSizeLong());
    while (remain > 0) {
        int ret = send(req->GetConn()->Socket(), buf, remain, 0);
        if (ret == 0) {
            mlog_info("conn close");
            gConnManager->CloseConn(req->GetConn()->Socket());
            return;
        } else if (ret < 0) {
            switch (errno) {
                case EAGAIN | EWOULDBLOCK:
                    break;
                case EINTR:
                    break;
                default:
                    gConnManager->CloseConn(req->GetConn()->Socket());
                    return;
            }
        } else {
            remain -= ret;
            buf += ret;
        }
    }
}
void do_request(std::queue<std::shared_ptr<MRequest>> reqs) {
    while (!reqs.empty()) {
        auto it = reqs.front();
        M::RpcHeader rpc_header;
        rpc_header.ParseFromBoundedZeroCopyStream(
            it->HeaderBuf().get(), it->HeaderBuf()->TotalBytes());
        reqs.pop();

        if (rpc_header.func() == "hello") {
            std::shared_ptr<M::HelloReq> req = std::make_shared<M::HelloReq>();
            std::shared_ptr<M::HelloRes> res = std::make_shared<M::HelloRes>();
            req->ParseFromBoundedZeroCopyStream(it->BodyBuf().get(),
                                                it->BodyBuf()->TotalBytes());
            mlog_info("req: " << req->SerializeAsString());
            HelloTask task(req, res);
            task.Run();

            rpc_header.set_type(M::RPC_TYPE_RPC_RESPONSE);
            do_response(it, rpc_header, res);
        }
    }
}

void recv_run(int fd) {
    MConnectionSP conn = gConnManager->GetConn(fd);
    if (nullptr == conn) {
        mlog_error(
            "conntion read data, but connection handler is not in "
            "connection set");
        gConnManager->CloseConn(fd);
        return;
    }
    static int32_t retry = 3;
    while (true) {
        MBufferSP buf;
        for (int i = 0; i < retry; i++) {
            buf = gbufpool->Allocate();
            if (buf != nullptr) break;
        }
        if (buf == nullptr) {
            mlog_error("cannot allocate buf from pool, close connection");
            gConnManager->CloseConn(fd);
            return;
        }

        int32_t len = recv(fd, buf->Buf(), buf->Remain(), 0);
        if (len < 0) {
            switch (errno) {
                case EAGAIN | EWOULDBLOCK:
                    gbufpool->GiveBack(buf);
                    return;
                case EINTR:
                    gbufpool->GiveBack(buf);
                    break;
                default:
                    gbufpool->GiveBack(buf);
                    gConnManager->CloseConn(fd);
                    return;
            }
        } else if (len > 0) {
            auto reqs = parse_connection(conn, buf, len);
            if (!reqs.empty()) {
                do_request(reqs);
            }
        } else {
            gbufpool->GiveBack(buf);
            gConnManager->CloseConn(fd);
            return;
        }
    }
}

void accept_run() {
    epfd = epoll_create(MAX_EVENTS);
    if (epfd < 0) {
        mlog_info("create epoll failed");
        return;
    }

    struct epoll_event event, ev;
    event.data.fd = sock;
    event.events = EPOLLIN | EPOLLOUT;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event) == -1) {
        mlog_info("epoll add failed.") close(epfd);
        return;
    }

    struct epoll_event events[MAX_EVENTS];
    struct sockaddr_in client_addr;
    socklen_t cliaddr_len = sizeof(client_addr);
    std::string client_ip;
    long client_port;

    mlog_info("start to wait connect.");
    while (true) {
        int fdset = epoll_wait(epfd, events, MAX_EVENTS, EVENT_TIMEOUT);
        for (int i = 0; i < fdset; ++i) {
            if (events[i].data.fd == sock) {
                int client =
                    accept(sock, (struct sockaddr *)&client_addr, &cliaddr_len);
                if (client < 0) {
                    mlog_error("accept failed. errno: " << errno);
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
                    mlog_info("epoll add client fd failed.");
                    close(client);
                    return;
                }
                gConnManager->CreateConn(client, client_ip, client_port);

            } else {
                if (events[i].events & EPOLLIN) {
                    recv_run(events[i].data.fd);
                }
                if (events[i].events & EPOLLOUT) {
                    // TODO
                }
                // signal_action(events[i].data.fd, SIGUSR2);
            }
        }
    }
}

static void daemon() {
    pid_t pid;
    pid = fork();
    if (pid < 0) exit(-1);
    if (pid > 0) exit(0);
}

static void listen_port(const int port, int &sock) {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        mlog_info("create socket failed.");
        return;
    }

    bzero(&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        mlog_info("bind failed.");
        close(sock);
        return;
    }

    if (listen(sock, MAX_EVENTS) < 0) {
        mlog_info("listen failed.");
        close(sock);
        return;
    }
}

static void set_multi_env() {
    struct rlimit limit;

    limit.rlim_cur = limit.rlim_max = 1024;
    if (setrlimit(RLIMIT_STACK, &limit) != 0) {
        mlog_info("set system stack size failed.");
        exit(0);
    }

    limit.rlim_cur = limit.rlim_max = 1000000;
    if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
        mlog_info("set system open file failed.");
        exit(0);
    }

    getrlimit(RLIMIT_STACK, &limit);
    if (limit.rlim_cur != 1024) {
        mlog_info("set system stack size failed.");
        exit(0);
    }

    getrlimit(RLIMIT_NOFILE, &limit);
    if (limit.rlim_cur != 1000000) {
        mlog_info("set system open file failed.");
        exit(0);
    }
}

static void watch_dog() {
    int cpunum = sysconf(_SC_NPROCESSORS_CONF);
    std::map<int, int> pool;

    std::queue<int> myqueue;
    for (int i = 0; i < cpunum; ++i) myqueue.push(i);

    cpu_set_t mask;
    while (true) {
        while (myqueue.size() > 0) {
            CPU_ZERO(&mask);
            CPU_SET(myqueue.front(), &mask);

            int pid = fork();
            if (pid == 0) {
                if (sched_setaffinity(0, sizeof(mask), &mask) < 0)
                    mlog_error("sched_setaffinity failed.");
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
