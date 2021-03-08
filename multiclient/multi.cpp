#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/syscall.h> 
#define gettid() syscall(__NR_gettid) 

#include <map>
#include <string>
#include <iostream>

#include "ip.h"
#include "http_task.h"

static int success = 0;
static int failed = 0;
static int port = 80;
static int procnum = 10;
static int thdnum = 100;
static std::string ip = "127.0.0.1";
static std::string test = TCP_TEST;

#ifdef DEBUG
static void signal_act(int signo)
{
	switch(signo)
	{
		case SIGABRT:
			std::cout << "exit sig: SIGABRT" << std::endl;
			break;
		case SIGCHLD:
			std::cout << "exit sig: SIGCHLD" << std::endl;
			break;
		case SIGTERM:
			std::cout << "exit sig: SIGTERM" << std::endl;
			break;
		default:
			std::cout << "exit sig: " << signo << std::endl;
			break;			
	}
}
#endif

static void *thread(void *param)
{
	pthread_detach(pthread_self());

    http_task(port, ip, test);

	pthread_exit(0);
	return 0;
}

static int thread_create()
{
	pthread_t pid[thdnum];
	std::map<pthread_t, bool> pool;
	for (int i = 0; i < thdnum; i++){
		int ret = pthread_create(&pid[i], NULL, thread, NULL);
		if(ret != 0){
#ifdef DEBUG
			std::cout << "create thread failed! errno: " << errno << std::endl;
#endif
			++failed;
			continue;
		}
		pool[pid[i]] = true;
		++success;
	}

	while(pool.size() > 0){
		for(int i = 0; i < thdnum; i++){
			int ret = pthread_kill(pid[i], 0); // 判断线程是否还存在
			if(ret == 0){ // 线程不存在
				if(pool.find(pid[i]) != pool.end()){
					pool.erase(pid[i]);
				}
			}
		}
		sleep(1);
	}

	std::cout << "child process [" << getpid() << "] exit, thread run success times: " << success  << " failed times: " << failed << std::endl;
	return 0;
}

static void usage()
{
	std::cout << "" << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << " .multi [options]" << std::endl;
	std::cout << " -I: server ip, default:127.0.0.1" << std::endl;
	std::cout << " -P: server port, range [1, 65535), default:80 " << std::endl;
	std::cout << " -p: process number, range [1, 20], defaut:10" << std::endl;
	std::cout << " -t: thread number, range [1, 2000], default:100" << std::endl;
	std::cout << " -H: HTTP test" << std::endl;
	std::cout << " -h: for help." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "if you have any questions, contract with me.(907792025@qq.com)" << std::endl;
	exit(0);
}

static void parse_args(int argc, char* argv[])
{
	int c = 0;
	while((c = getopt(argc, argv, "hI:P:p:t:H")) != -1)
	{
		switch(c){
			case 'I':
				ip = optarg;
				break;
			case 'P':
				port = atoi(optarg);
				break;
			case 'p':
				procnum = atoi(optarg);
				break;
			case 't':
				thdnum = atoi(optarg);
				break;
			case 'H':
				test = HTTP_TEST;
				break;
			case 'h':
				usage();
				break;
			default:
				break;
		}
	}

	if(ipvalid(ip.c_str()) == false || port < 0 || port > 65535 || procnum > 20 || procnum < 1 || thdnum < 1 || thdnum > 2000){
		usage();
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

	limit.rlim_cur = limit.rlim_max = 100000;
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
	if(limit.rlim_cur != 100000){
		std::cout << "set system open file failed." << std::endl;
		exit(0);
	}
}

int main(int argc, char* argv[])
{
#ifdef DEBUG
	struct sigaction sig;
	sig.sa_flags = 0;
	sig.sa_handler = signal_act;
	
	sigaction(SIGCHLD, &sig, NULL);
	sigaction(SIGTERM, &sig, NULL);
	sigaction(SIGABRT, &sig, NULL);
#endif

	parse_args(argc, argv);
	set_multi_env();

	int pid = -1;
	std::map<int, bool> pool;
	for(int i = 0; i < procnum; i++){
		pid = fork();
		if(pid <= 0){
			break;
		}
		pool[pid] = true;
	}

	if(pid == 0){
		thread_create();
    }else if(pid > 0){
        while(pool.size() > 0) {
            for(auto item = pool.begin(); item != pool.end();) {
                int ret = waitpid(item->first, NULL, 0); // 等待子进程执行完
                if (ret == item->first) {
                    pool.erase(item++);
                } else {
                    item++;
                }
            }

            std::cout << "main  process [" << getpid() << "] loop, wait child exit, alive: " << pool.size() << std::endl;
			sleep(1);
        }
	}
	return 0;
}
