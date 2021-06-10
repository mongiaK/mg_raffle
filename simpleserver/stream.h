/*================================================================
*
*  文件名称：stream.h
*  创 建 者: mongia
*  创建日期：2021年06月04日
*
================================================================*/

#pragma once

#include "buffer.h"
#include "server.h"
#include "connection.h"
#include "ss.h"
#include "stream_parser.h"
#include <asm-generic/errno-base.h>
#include <atomic>

class SStream {
   public:
    SStream(SConnectionSP conn, SConnectionStatSP conn_stat, SServerSP serversp)
        : _connsp(conn),
          _conn_statsp(conn_stat),
          _serversp(serversp),
            _buf(_serversp->get_buf_pool(), 2048) {
        _recieve_size = 0;
    }
    ~SStream() {};

    void on_read_some() {
        if (_buf->get_remain_size() == 0) {
            _buf->reset();
        }
        int len = recv(_connsp->get_socket(), _buf->get_buf(), _buf->get_remain_size(), 0);
        if (len == 0) {
            _serversp->release_connection(_connsp); 
        } else if (len < 0) {
            switch(errno) {
                case EAGAIN :
                    break;
                case EINTR:
                    on_read_some();
                    break;
                default:
                    _serversp->release_connection(_connsp);
                    break;
            }
        } else {
            _recieve_size += len;
            _parser->parse_msg(_buf->get_buf(), len);
            _buf->usage(len);
        }
    }

    void on_write_some() {}

    void on_close() {
        close(_connsp->get_socket());
    }

   private:
    SConnectionSP _connsp;
    SConnectionStatSP _conn_statsp;
    SServerSP _serversp;
    SBufferGuard _buf;
    CSTreamParserSP _parser;

    std::atomic<int> _recieve_size;
};

