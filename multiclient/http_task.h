/*================================================================
*  
*  文件名称：http_task.h
*  创 建 者: mongia
*  创建日期：2021年03月08日
*  
================================================================*/
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

#include <iostream>

#define TCP_TEST "hello"
#define HTTP_TEST "POST / HTTP/1.1\r\n"															\
				  "Accept: */*\r\n"																\
				  "host: localhost\r\n"															\
				  "Content-Type: application/x-www-form-urlencoded\r\n"							\
				  "Content-Length: 5\r\n"														\
				  "Connection: close\r\n\r\n"													\
				  "hello"

void http_task(int port, std::string ip, std::string content)
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
