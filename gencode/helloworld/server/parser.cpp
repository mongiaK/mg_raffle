/*================================================================
*
*  文件名称：parser.cpp
*  创 建 者: mongia
*  创建日期：2023年04月23日
*  邮    箱：mongiaK@outlook.com
*
================================================================*/

#include "parser.h"

#include <string.h>

#include <memory>

#include "connection.h"
#include "request.h"

MParser::MParser(MConnectionSP conn) : _conn(conn) {
    _header_remain = sizeof(struct MLengthHeader);
    _req = std::make_shared<MRequest>(conn);
}
MParser::~MParser() {}

MRequestSP MParser::SplitMessage(MBufferSP buf, int& length) {
    int remain = 0;
    while (length > 0) {
        switch (_conn->ParseType()) {
            case M_HEADER:
                remain = std::min(_header_remain, length);
                memcpy(_req->LengthHeader() + sizeof(MLengthHeader) -
                           _header_remain,
                       buf->Buf(), remain);
                buf->Consume(remain);
                _header_remain -= remain;
                length -= remain;

                if (_header_remain > 0) break;
                _conn->ChangeParseType(M_RPC_HEADER);
                _rpc_remain = _req->RPCHeaderLength();

                break;
            case M_RPC_HEADER:
                remain = std::min(_rpc_remain, length);
                _req->HeaderBuf()->Append(
                    std::make_shared<MBufferGuard>(buf, remain));
                buf->Consume(remain);
                _rpc_remain -= remain;
                length -= remain;

                if (_rpc_remain > 0) break;
                _conn->ChangeParseType(M_RPC_CONTENT);
                _rpc_remain = _req->RPCBodyLength();

                break;
            case M_RPC_CONTENT:
                remain = std::min(_rpc_remain, length);
                _req->BodyBuf()->Append(
                    std::make_shared<MBufferGuard>(buf, remain));
                buf->Consume(remain);
                _rpc_remain -= remain;
                length -= remain;

                if (_rpc_remain > 0) break;

                MRequestSP req = std::make_shared<MRequest>(_conn);
                _req.swap(req);
                Reset();
                return req;
        }
    }
    return nullptr;
}

void MParser::Reset() {
    _header_remain = sizeof(MLengthHeader);
    _rpc_remain = 0;
    _conn->ChangeParseType(M_MAGIC);
}
