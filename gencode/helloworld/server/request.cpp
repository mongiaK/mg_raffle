/*================================================================
*  
*  文件名称：request.cpp
*  创 建 者: mongia
*  创建日期：2023年04月25日
*  邮    箱：mongiaK@outlook.com
*  
================================================================*/

#include "request.h"
#include "buffer_manager.h"

MRequest::MRequest(MConnectionSP conn)
    :_conn(conn)
     , _rpc_header_buf(new MReadBuf())
     , _rpc_body_buf(new MReadBuf())
{}

MRequest::~MRequest()
{

}
