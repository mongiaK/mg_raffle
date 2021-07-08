/*================================================================
*
*  文件名称：stream.h
*  创 建 者: mongia
*  创建日期：2021年06月04日
*
================================================================*/

#pragma once

#include <asm-generic/errno-base.h>

#include <atomic>
#include <cstring>
#include <deque>

#include "buffer.h"
#include "connection.h"
#include "parser.h"
#include "binary_stream_parser.h"
#include "server.h"
#include "ss.h"

class SStream {
   public:
    SStream(SConnectionSP conn, SServerSP serversp)
        : _connsp(conn),
          _serversp(serversp),
          _magic_string_recieve_size(0) {}
    ~SStream(){};

    void register_request_callback(RequestCallback callback) {
        _request_callback = callback;
    }

    void on_read_some() {
        SBufferGuardSP buf(new SBufferGuard(_serversp->get_buf_pool(), 128));
        int len = recv(_connsp->get_socket(), (*buf.get())->get_buf(),
                       (*buf.get())->get_remain_size(), 0);
        if (len == 0) {  // connection close by peer
            _serversp->release_connection(_connsp);
            return;
        } else if (len < 0) {
            switch (errno) {
                case EAGAIN:  // no data to read
                    return;
                case EINTR:  // read interraupt by system
                    break;
                default:  // error happen
                    _serversp->release_connection(_connsp);
                    return;
            }
        } else {
            _connsp->fresh_stat(len);
            std::deque<SRequestSP> requests;
            if (!split_message(buf, len, requests)) {
                _serversp->release_connection(_connsp);
                return;
            }

            while(!requests.empty()) {
                auto request = requests.front();
                requests.pop_front();
                _request_callback(request);
            }
        }

        // read again
        on_read_some();
    }

    bool split_message(SBufferGuardSP bufsp, int remain_length, std::deque<SRequestSP>& requests) {
        const char* buf = (*bufsp.get())->get_buf();
        int consume = 0;
        int real_read = 0;
        while (remain_length > 0) {
            if (_magic_string_recieve_size < 4) {
                real_read =
                    std::min(4 - _magic_string_recieve_size, remain_length);
                memcpy(reinterpret_cast<char*>(&_magic_number) +
                           _magic_string_recieve_size,
                       buf + consume, real_read);
                _magic_string_recieve_size += real_read;
                consume += real_read;
                if (_magic_string_recieve_size < 4) {
                    // magic has read incomplete
                    return true;
                }
            }

            if (!_parser || choose_parser()) {
                return false;
            }

            int ret = _parser->parser_msg(bufsp, consume, remain_length);
            if (ret == 0) {
                return true;
            } else if (ret < 0) {
                return false;
            }

            SRequestSP request = _parser->get_request();
            request->set_recieve_time(time(NULL));
            request->set_remote_ip(_connsp->get_dst_ip());
            request->set_remote_port(_connsp->get_dst_port());
            requests.push_back(request);
        }

        return true;
    }

    bool choose_parser() {
        switch (_magic_number) {
            case 1:
                _parser = SBinarySTreamParserSP(new SBinarySTreamParser());
                break;
            default:
                return false;
                break;
        }
        return true;
    }

    void on_write_some() {}

    void on_close() { close(_connsp->get_socket()); }

   private:
    RequestCallback _request_callback;
    SConnectionSP _connsp;
    SServerSP _serversp;
    SParserSP _parser;
    int _magic_string_recieve_size;
    int _magic_number;
};

