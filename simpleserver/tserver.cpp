#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <signal.h>
#include <pthread.h>
#include <sched.h>

#define MAX_EVENTS     1024
#define EVENT_TIMEOUT  10

static struct sockaddr_in server;
static int sock = -1;
static int epfd = -1;

struct hodor
{
	int client;
	std::string msg;
};

void recv_run(int fd);

void setnonblock(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);  
	flags |= O_NONBLOCK;  
	fcntl(fd, F_SETFL, flags); 
}

void dowork(int sig, siginfo_t * siginfo, void* data)
{
	struct hodor* dor = NULL;	
	switch(sig){
		case SIGUSR1:
			dor = (struct hodor*)siginfo->si_value.sival_ptr;
			std::cout << "recv sig: " << sig << ", fd: " << dor->client << ", msg: " << dor->msg.c_str() << std::endl;
			free(dor);
			dor = NULL;
			break;
		case SIGUSR2:
			recv_run(siginfo->si_value.sival_int);
			break;
	}
}

void register_signal(int signo)
{
	struct sigaction act;
	
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = dowork;
	act.sa_flags = SA_SIGINFO;

	if(sigaction(signo, &act, NULL) < 0){
		std::cerr << "install signal failed" << std::endl;
	}
}

void signal_action(int fd, int signo)
{
	union sigval myval;
	
	myval.sival_int = fd;
	sigqueue(getpid(), signo, myval);	
}

static int read_times = 0;
void recv_run(int fd)
{
	std::string recv_buf = "";
	do{
		char buf[10] = {0};
		int len = recv(fd, buf, sizeof(buf)-1, 0);
		if(len < 0){
			switch(errno){
                case EAGAIN || EWOULDBLOCK:
                     return;
                case EINTR:
                     break;
				default:
					goto conn_close;
			}
		}else if(len > 0){
			buf[len] = '\0';
			recv_buf += buf;
			std::cout << "client fd: " << fd << ", recv data: " << recv_buf.c_str() << ", times: " << ++read_times << std::endl;
		}else{
            goto conn_close;
		}
	}while(true);

conn_close:
    
    struct epoll_event event;
    event.data.fd = fd;
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event);
    
    close(fd);
    std::cerr << "client closed" << std::endl;
}

void accept_run()
{
	epfd = epoll_create(MAX_EVENTS);
	if(epfd < 0){
		std::cout << "create epoll failed" << std::endl;
		return;
	}

	struct epoll_event event, ev;
	event.data.fd = sock;
	event.events = EPOLLIN | EPOLLOUT;

	if(epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event) == -1){
		std::cout << "epoll add failed." << std::endl;
		close(epfd);
		return;
	}

	struct epoll_event events[MAX_EVENTS];
	struct sockaddr_in client_addr; 
	socklen_t cliaddr_len = sizeof(client_addr);

	std::cout << "start to wait connect." << std::endl;
	while(true){
		int fdset = epoll_wait(epfd, events, MAX_EVENTS, EVENT_TIMEOUT);
		for(int i = 0; i < fdset; ++i){
			if(events[i].data.fd == sock){
				int client = accept(sock, (struct sockaddr*)&client_addr, &cliaddr_len);
				if(client < 0){
					std::cerr << "accept failed. errno: " << errno << std::endl;
					continue;
				}
				ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
				ev.data.fd = client;
				setnonblock(client);
				if(epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev) < 0){
					std::cerr << "epoll add client fd failed." << std::endl;
					close(epfd);
					return;
				}
			}else{
				recv_run(events[i].data.fd);
				//signal_action(events[i].data.fd, SIGUSR2);
			}
		}
	}
	
}

static void daemon()
{
	pid_t pid;
	pid = fork();
	if(pid < 0) exit(-1);
	if(pid > 0) exit(0);
}

static void listen_port(const int port, int& sock)
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		std::cout << "create socket failed." << std::endl;
		return;
	}

	bzero(&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	if(bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0){
		std::cout << "bind failed." << std::endl;
		close(sock);
		return;
	}

	if(listen(sock, MAX_EVENTS) < 0){
		std::cout << "listen failed." << std::endl;
		close(sock);
		return;
	}
}

static void set_multi_env()
{
	struct rlimit limit;

	limit.rlim_cur = limit.rlim_max = 1024;
	if(setrlimit(RLIMIT_STACK, &limit) != 0){
		std::cout << "set system stack size failed." << std::endl;
		exit(0);
	}

	limit.rlim_cur = limit.rlim_max = 1000000;
	if(setrlimit(RLIMIT_NOFILE, &limit) != 0){
		std::cout << "set system open file failed." << std::endl;
		exit(0);
	}

	getrlimit(RLIMIT_STACK, &limit);
	if(limit.rlim_cur != 1024){
		std::cout << "set system stack size failed." << std::endl;
		exit(0);
	}

	getrlimit(RLIMIT_NOFILE, &limit);
	if(limit.rlim_cur != 1000000){
		std::cout << "set system open file failed." << std::endl;
		exit(0);
	}
}

static void watch_dog()
{
	int cpunum = sysconf(_SC_NPROCESSORS_CONF);
	std::map<int, int> pool;
	
	std::queue<int> myqueue;
	for(int i = 0; i < cpunum; ++i) myqueue.push(i);

	cpu_set_t mask;
	while(true){
		while(myqueue.size() > 0){
			CPU_ZERO(&mask);
			CPU_SET(myqueue.front(), &mask);

			int pid = fork();
			if(pid == 0){
				if(sched_setaffinity(0, sizeof(mask), &mask) < 0)
					std::cerr << "sched_setaffinity failed." << std::endl;
				return;
			}else if(pid < 0){
				exit(1);
			}

			pool[myqueue.front()] = pid;
			myqueue.pop();
		}
		for(auto &its : pool){
			int ret = waitpid(its.second, NULL, WNOHANG);
			if(ret == its.second){
				pool[its.first] = 0;
				myqueue.push(its.first);
			}
		}
		sleep(1);
	}
}

int main(int argc, char* argv[])
{
	daemon();
	set_multi_env();

	register_signal(SIGPIPE);
	register_signal(SIGILL);

	listen_port(8090, sock);
	
	watch_dog();

	accept_run();

	close(sock);
	return 0;
}
