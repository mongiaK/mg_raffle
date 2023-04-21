/*================================================================
*
*  文件名称：server.h
*  创 建 者: mongia
*  创建日期：2021年06月03日
*
================================================================*/

#pragma once

#include <functional>

#include "buffer.h"
#include "connection.h"
#include "epoll_connection.h"
#include "epoll_event.h"
#include "event.h"
#include "frequecy.h"
#include "log.h"
#include "server.h"
#include "ss.h"
#include "tcp_listen.h"

class STCPServer : public SServer {
   public:
    STCPServer(std::string address, int port)
        : _server_addr(address), _server_port(port) {}

    virtual bool init() {
        _buf_poolsp.reset(new SBufferPool());

        _eventsp = SEpollEventSP(new SEpollEvent());
        _listensp =
            STCPListenSP(new STCPListen(_server_addr, _server_port, _eventsp));

        _listensp->register_accept_callback(std::bind(
            &STCPServer::accept_callback, this, std::placeholders::_1));
        return true;
    }

    virtual std::string get_server_address() { return _server_addr; }

    virtual int get_server_port() { return _server_port; }

    virtual bool start() {
        if (!init()) {
            slog_fatal("server init failed.");
        }
        if (!_eventsp->init()) {
            return false;
        }        
        if (!_listensp->start_listen()) {
            return false;
        }

        _listensp->register_listen_event();

        _eventsp->event_run();
        return true;
    }

    void accept_callback(SEventInfoSP info) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int clientfd =
            accept(info->_fd, (struct sockaddr*)&client_addr, &client_len);
        if (clientfd <= 0) {
            slog_error("accept connect fail. " << strerror(errno));
            return;
        }

        SConnectionSP conn = create_connection(clientfd, _eventsp);
        conn->set_dst_ip(inet_ntoa(client_addr.sin_addr));
        conn->set_dst_port(ntohl(client_addr.sin_port));
        conn->register_request_callback(std::bind(&STCPServer::request_callback,
                                                  this, std::placeholders::_1));

        if (!check_connection_avalible(conn)) {
            slog_warn("reject connect: " << conn->get_dst_ip() << ":"
                                         << conn->get_dst_port());
            return;
        }

        if (conn->register_event()) {  // add client to event queue
            _conns[clientfd] = conn;
        }

        slog_debug("accept connection" << conn->get_dst_ip()
                                       << ", remotefd: " << conn->get_socket());
    }

    void request_callback(SRequestSP request) { request->process_request(); }

    virtual SConnectionSP create_connection(int clientfd, SEventSP eventsp) {
        SEpollConnectionSP conn(
            new SEpollConnection(clientfd, eventsp, STCPServerSP(this)));
        return conn;
    }

    virtual SBufferPoolSP get_buf_pool() { return _buf_poolsp; }

    bool check_connection_avalible(SConnectionSP conn) { return true; }

    void check_connection_stat() {
        // TODO check connection, to kill idle connection
    }

    virtual void release_connection(SConnectionSP conn) {
        conn->on_close();
        _conns.erase(conn->get_socket());
    }

   private:
    std::string _server_addr;
    int _server_port;

    SListenSP _listensp;
    SEventSP _eventsp;
    Frequency _frequency;                 // 频率控制
    std::map<int, SConnectionSP> _conns;  // 连接集合
    SBufferPoolSP _buf_poolsp;
};
