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

#include <memory>
struct MagicHeader {
    union magic {
        char _str[4];
        int32_t _val;
    };

    magic _mag;
};

struct SizeHeader {
    int32_t _total_size;
    int32_t _rpc_header_size;
    int32_t _rpc_content_size;
};

enum ReadType {
    MAGIC_HEADER = 1,
    SIZE_HEADER = 2,
    RPC_HEADER = 3,
    RPC_CONTENT = 4,
};

static int g_magic_size = sizeof(struct MagicHeader);
static int g_size = sizeof(struct SizeHeader);

class CSTreamParser {
   public:
    CSTreamParser() : _type(MAGIC_HEADER){};
    virtual ~CSTreamParser(){};

    void parse_msg(const char* buf, int length) {
        int need_read = 0;
        int remain_length = length;
        switch (_type) {
            case MAGIC_HEADER:
                assert(_module_has_read_size < g_magic_size);
                need_read = g_magic_size - _module_has_read_size;
                if (remain_length <
                    need_read) {  // no enough magic data to read
                    memcpy((char*)(&_header) + _module_has_read_size, buf, remain_length);
                    _module_has_read_size += remain_length;
                    return;
                }
                memcpy(&_header._mag + _module_has_read_size, buf, need_read);
                _type = SIZE_HEADER;
                _module_has_read_size = 0;
                remain_length -= need_read;
            case SIZE_HEADER:
                assert(_module_has_read_size < g_size);
                need_read = g_size - _module_has_read_size;
                if (remain_length < need_read) {
                    memcpy((char*)(&_size_header) + _module_has_read_size, buf,
                           remain_length);
                    _module_has_read_size += remain_length;
                    return;
                }
                memcpy((char*)(&_size_header) + _module_has_read_size, buf, need_read);
                _type = RPC_HEADER;
                _module_has_read_size = 0;
                remain_length -= need_read;
            case RPC_HEADER:
                assert(_module_has_read_size < _size_header._rpc_header_size);
                need_read = _size_header._rpc_header_size - _module_has_read_size;
                if (remain_length < need_read) {
                    
                }
                break;

            case RPC_CONTENT:
                break;
        }
    }

   private:
    MagicHeader _header;
    SizeHeader _size_header;
    ReadType _type;
    int _module_has_read_size; // module has read in buffer;
};

typedef std::shared_ptr<CSTreamParser> CSTreamParserSP;
