/*================================================================
*
*  文件名称：connection.h
*  创 建 者: mongia
*  创建日期：2023年04月23日
*  邮    箱：mongiaK@outlook.com
*
================================================================*/

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <memory>
#include <string>

#include "buffer_manager.h"
#include "util.h"

class MConnection {
   public:
    MConnection(int32_t& fd, std::string& client_ip, long& client_port);
    ~MConnection();

    int32_t Socket() const {
        return _fd;
    }

    int32_t MagicRemain() const {
        return _magic_remain;
    }

    int32_t* Magic() {
        return &_magic;
    }

    void MagicRemain(int32_t remain) {
        _magic_remain -= remain;
    }

    void ChangeParseType(int32_t parse_type) {
        _parse_type = parse_type;
    }

    int32_t ParseType() const {
        return _parse_type;
    }

    void ResetParser(MParserSP parser) {
        _parser = parser;
    }

    bool HasParser() {
        return (_parser != nullptr);
    }

    MParserSP Parser() const  {
        return _parser;
    }

    void SendQueue(MBufferGuardSP bufguard) {
        _send_buf->Append(bufguard);
    }

    void SendBuf();

   private:
    int32_t _fd;
    std::string _client_ip;
    long _client_port;
    int32_t _magic;
    int32_t _magic_remain;
    int32_t _parse_type;
    MParserSP _parser;

    MReadBufSP _send_buf;
};


#endif  // __CONNECTION_H__
