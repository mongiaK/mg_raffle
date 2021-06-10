/*================================================================
*
*  文件名称：epoll_connection.h
*  创 建 者: mongia
*  创建日期：2021年06月03日
*
================================================================*/

#pragma once

#include <assert.h>
#include <bits/stdint-intn.h>

#include <list>
#include <map>
#include <memory>
#include <mutex>

#include "ss.h"
#include "common.h"
#include "connection.h"
#include "stream.h"
#include "server.h"
#include "event.h"

class SEpollConnection : public SConnection {
   public:
    SEpollConnection(int fd, SEventSP eventsp, SServerSP serversp)
        : _sockfd(fd), _eventsp(eventsp),_serversp(serversp) {
        setnonblock(fd);

        _conn_statsp.reset(new SConnectionStat());
        _streamsp.reset(new SStream(SEpollConnectionSP(this), _conn_statsp, _serversp));

        _event_infosp->_fd = _sockfd;
        _event_infosp->_events = EPOLLIN | EPOLLOUT | EPOLLET;
        _event_infosp->_callback = std::bind(&SEpollConnection::conn_callback,
                                             this, std::placeholders::_1);
    }
    ~SEpollConnection() {}
    void set_dst_ip(char* dst_ip) { _dst_ip = dst_ip; }

    void set_dst_port(int dst_port) { _dst_port = dst_port; }

    std::string get_dst_ip() const { return _dst_ip; }

    int get_dst_port() const { return _dst_port; }
    
    int get_socket() const { return _sockfd; }

    bool register_event() { return _eventsp->add_event(_event_infosp); }
    bool remove_event() { return _eventsp->remove_event(_event_infosp); }

    void on_close() {
        _streamsp->on_close();
    }

    void conn_callback(SEventInfoSP info) {
        if (info->_events & EPOLLIN) {
            _streamsp->on_read_some();
        } else if (info->_events & EPOLLOUT) {
            _streamsp->on_write_some();
        }
    }

   private:
    int _sockfd;          // 连接socket
    std::string _dst_ip;  // 目的ip
    int _dst_port;        // 目的端口

    SStreamSP _streamsp;
    SEventSP _eventsp;
    SEventInfoSP _event_infosp;
    SConnectionStatSP _conn_statsp;
    SServerSP _serversp;
};

