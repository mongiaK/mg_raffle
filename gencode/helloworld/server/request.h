/*================================================================
*  
*  文件名称：request.h
*  创 建 者: mongia
*  创建日期：2023年04月24日
*  邮    箱：mongiaK@outlook.com
*  
================================================================*/

#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <memory>

#include "rpc.pb.h"
#include "util.h"

#include "buffer_manager.h"

class MRequest {
  public:
    MRequest(MConnectionSP conn);
    ~MRequest();

    MConnectionSP GetConn() { return _conn; }

    char* LengthHeader() {
        return (char*)(&_length_header);
    }

    int32_t RPCHeaderLength() const {
        return _length_header.rpc_header_length;
    }

    int32_t RPCBodyLength() const {
        return _length_header.rpc_body_length;
    }

    MReadBufSP HeaderBuf() const {
        return _rpc_header_buf;
    }

    MReadBufSP BodyBuf() const {
        return _rpc_body_buf;
    }

  private:
    MConnectionSP _conn;
    struct MLengthHeader _length_header; 
    M::RpcHeader _rpc_header;
    MReadBufSP _rpc_header_buf;
    MReadBufSP _rpc_body_buf;
};

typedef std::shared_ptr<MRequest> MRequestSP;


#endif // __REQUEST_H__
