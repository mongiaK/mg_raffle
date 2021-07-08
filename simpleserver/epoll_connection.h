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

#include "common.h"
#include "connection.h"
#include "event.h"
#include "server.h"
#include "ss.h"
#include "stream.h"

class SEpollConnection : public SConnection {
   public:
    SEpollConnection(int fd, SEventSP eventsp, SServerSP serversp)
        : _sockfd(fd), _eventsp(eventsp), _serversp(serversp) {

        _conn_statsp.reset(new SConnectionStat());
        _streamsp.reset(new SStream(SEpollConnectionSP(this), _serversp));

        setnonblock(fd);
        _event_infosp.reset(
            new SEventInfo(_sockfd, EPOLLIN | EPOLLOUT | EPOLLET,
                           std::bind(&SEpollConnection::conn_callback, this,
                                     std::placeholders::_1)));
    }
    ~SEpollConnection() {}
    void set_dst_ip(char* dst_ip) { _dst_ip = dst_ip; }

    void set_dst_port(int dst_port) { _dst_port = dst_port; }

    std::string get_dst_ip() const { return _dst_ip; }

    int get_dst_port() const { return _dst_port; }

    int get_socket() const { return _sockfd; }

    void register_request_callback(RequestCallback callback) {
        _streamsp->register_request_callback(callback);
    }

    bool register_event() { return _eventsp->add_event(_event_infosp); }
    bool remove_event() { return _eventsp->remove_event(_event_infosp); }

    void on_close() { 
        remove_event();
        _streamsp->on_close(); 
    }

    void conn_callback(SEventInfoSP info) {
        if (info->_events & EPOLLIN) {
            _streamsp->on_read_some();
        } else if (info->_events & EPOLLOUT) {
            _streamsp->on_write_some();
        }
    }

    void fresh_stat(int len) { _conn_statsp->refresh_stat(len); }

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

