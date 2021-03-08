/*================================================================
*
*  文件名称：network.h
*  创 建 者: mongia
*  创建日期：2020年10月26日
*  描    述：mr_pengmj@outlook.com
*
================================================================*/

#pragma once

#include <iostream>
#include <cerrno>
#include <thread>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

class Network {
   private:
    int _srfd;
    int _port;
    std::string _srip;
   public:
    Network(int port, std::string& server_ip) : _srfd(-1), _port(port), _srip(server_ip) {}
    ~Network() {
        if (_srfd > 0) {
            close(_srfd);
        }
    };

    bool create_server() {
        int _srfd = socket(AF_INET, SOCK_STREAM, 0);
        if(_srfd < 0) {
            std::cerr << "create socket failed. err: " << errno << std::endl;
            return false;
        }

        struct sockaddr_in sr_addr;
        sr_addr.sin_port = htonl(_port);
        sr_addr.sin_addr.s_addr = inet_addr(_srip.c_str());
        sr_addr.sin_family = AF_INET;

        bind(_srfd, (struct sockaddr*)(&sr_addr), sizeof(struct sockaddr));

        return true;
    }

    bool setsockopt() {
        return true;
    }

    bool setrlimit() {
        return true;
    }

    void start_server() {
        listen(_srfd, 1024);

        std::thread run(start_accept);
    }

    static void start_accept() {

    }

};
