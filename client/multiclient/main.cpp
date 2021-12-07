#include <stdio.h>
#include <unistd.h>
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
#include <sys/syscall.h> 
#define gettid() syscall(__NR_gettid) 

#include <map>
#include <string>
#include <iostream>

#include "ip.h"
#include "multi.h"

#define TCP_TEST "hello"
#define HTTP_TEST "POST / HTTP/1.1\r\n"															\
				  "Accept: */*\r\n"																\
				  "host: localhost\r\n"															\
				  "Content-Type: application/x-www-form-urlencoded\r\n"							\
				  "Content-Length: 5\r\n"														\
				  "Connection: close\r\n\r\n"													\
				  "hello"

static int port = 80;
static int procnum = 10;
static int thdnum = 100;
static std::string ip = "127.0.0.1";
static std::string content = TCP_TEST;

void http_task()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		std::cerr << "create sock failed" << std::endl;
		return ;
	}

	struct sockaddr_in server;
	bzero(&server, sizeof(sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(ip.c_str());

	if(connect(sock, (struct sockaddr*)&server, sizeof(struct sockaddr)) < 0){
		std::cerr << "connect server failed." << std::endl; 
		close(sock);
		return ;
	}

	send(sock, content.c_str(), content.length() + 1, 0);
	close(sock);

	return ;
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
				content = HTTP_TEST;
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

int main(int argc, char* argv[])
{
	parse_args(argc, argv);

    register_func(http_task);

    multi_create(procnum, thdnum); 

	return 0;
}
