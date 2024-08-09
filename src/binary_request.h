/*================================================================
*
*  文件名称：binary_request.h
*  创 建 者: mongia
*  创建日期：2021年06月30日
*
================================================================*/

#pragma once

#include "buffer.h"
#include "request.h"
#include <cstdint>
#include <memory>

struct RpcMessageHeader {
    int32_t _meta_size;
    int32_t _data_size;
    int32_t _message_size; // message_size = meta_size + data_size
};

class SBinaryRequest : public SRequest {
  public:
    SBinaryRequest() : _recieved_time(0) {
        _read_bufsp.reset(new ReadBuffer());
    }

    virtual ~SBinaryRequest() {}

    ReadBufferSP get_read_buf() { return _read_bufsp; };

    void process_request() {}

    void set_recieve_time(int64_t tm) { _recieved_time = tm; }

    void set_remote_ip(const std::string &ip) { _remote_ip = ip; }

    void set_remote_port(const int32_t port) { _remote_port = port; }

  private:
    RpcMessageHeader _rpc_header;
    ReadBufferSP _read_bufsp;
    int64_t _recieved_time;
    std::string _remote_ip;
    int32_t _remote_port;
};

typedef std::shared_ptr<SBinaryRequest> SBinaryRequestSP;
