/*================================================================
*  
*  文件名称：tcp_listen.h
*  创 建 者: mongia
*  创建日期：2021年06月03日
*  
================================================================*/

#pragma once

#include "log.h"
#include "common.h"
#include "listen.h"
#include "epoll_event.h"
#include "event.h"

class STCPListen : public SListen {
   public:
    STCPListen(std::string address, int port, SEventSP eventsp, int listen_size = 1024)
        : _listen_size(listen_size),
          _listen_addr(address),
          _listen_port(port),
          _eventsp(eventsp){
          }
    ~STCPListen() {
        if (_listenfd > 0) {
            close(_listenfd);
        }
    }
    
    virtual void register_accept_callback(EventCallback callback) {
        _accept_callback = callback;
    }

    virtual bool start_listen() {
        int sockfd = socket(PF_INET, SOCK_STREAM, 0);

        if (sockfd < 0) {
            slog_error("create listen socket failed. " << strerror(errno));
            return false;
        }

        struct sockaddr_in addr;
        addr.sin_family = PF_INET;
        addr.sin_addr.s_addr = inet_addr(_listen_addr.c_str());
        addr.sin_port = htons(_listen_port);

        int ret =
            bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr));
        if (ret < 0) {
            slog_error("bind addr failed. " << strerror(errno));
            return false;
        }

        listen(sockfd, _listen_size);

        _listenfd = sockfd;

        slog_info("server listen " << _listen_addr << ":" << _listen_port
                  << " ready!");
        return true;
    }

    virtual void register_listen_event() {
        setnonblock(_listenfd);
        _event_infosp.reset(new SEventInfo(_listenfd, EPOLLIN | EPOLLET, _accept_callback));
        _eventsp->add_event(_event_infosp);
    }

    virtual int get_socket() const { return _listenfd; }

   private:
    int _listenfd;
    int _listen_size;
    std::string _listen_addr;
    int _listen_port;

    SEventSP _eventsp;
    EventCallback _accept_callback;
    SEventInfoSP _event_infosp;
};

