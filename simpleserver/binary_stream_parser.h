/*================================================================
*
*  文件名称：stream_parser.h
*  创 建 者: mongia
*  创建日期：2021年06月10日
*
================================================================*/

#pragma once

#include <assert.h>
#include <bits/stdint-intn.h>
#include <string.h>
#include <unistd.h>

#include <memory>

#include "binary_request.h"
#include "buffer.h"
#include "parser.h"

struct RpcMessageHeader {
    union {
        char _str[4];
        int32_t _magic_str;
    };

    int32_t _meta_size;
    int32_t _data_size;
    int32_t _message_size;  // message_size = meta_size + data_size
};

enum ReadType {
    RPC_HEADER = 1,
    RPC_MESSAGE = 2,
};

static int g_header_size = sizeof(RpcMessageHeader);

class SBinarySTreamParser : public SParser {
   public:
    SBinarySTreamParser() { reset_recieve_env(); };
    virtual ~SBinarySTreamParser(){};

    void reset_recieve_env() {
        _module_has_read_size = 0;
        _type = RPC_HEADER;
        _requestsp.reset(new SBinaryRequest());
    }

    int parser_msg(SBufferGuardSP bufsp, int& consume, int& remain_length) {
        const char* buf = (*bufsp.get())->get_buf();
        int real_read = 0;
        switch (_type) {
            case RPC_HEADER:
                assert(_module_has_read_size < g_header_size);

                real_read = std::min(g_header_size - _module_has_read_size,
                                     remain_length);
                memcpy((char*)(&_recieve_header) + _module_has_read_size,
                       buf + consume,
                       real_read);  // TODO if endian is diff, error will
                                    // happen

                _module_has_read_size += real_read;
                consume += real_read;
                remain_length -= real_read;

                if (_module_has_read_size <
                    g_header_size) {  // head read incomplete
                    return 0;
                }

                if (_recieve_header._message_size !=
                    (_recieve_header._meta_size + _recieve_header._data_size)) {
                    return -1;
                }

                _module_has_read_size = 0;

                _type = RPC_MESSAGE;
                if(remain_length == 0) {
                    return 0;
                }
            case RPC_MESSAGE:
                real_read = std::min(
                    _recieve_header._message_size - _module_has_read_size,
                    remain_length);
                _requestsp->get_read_buf()->Append(BufHandle(consume, real_read, bufsp));

                _module_has_read_size += real_read;
                consume += real_read;
                remain_length -= real_read;
                if (_module_has_read_size <
                    _recieve_header._message_size) {  // message read incomplete
                    return 0;
                }

                // TODO msg has read complete, do function
                reset_recieve_env();
                break;
        }
        return 1;
    }

    SRequestSP get_request() {
        return _requestsp;
    }

   private:
    RpcMessageHeader _recieve_header;
    ReadType _type;
    SRequestSP _requestsp;

    int _module_has_read_size;  // module has read in buffer;
};

typedef std::shared_ptr<SBinarySTreamParser> SBinarySTreamParserSP;
