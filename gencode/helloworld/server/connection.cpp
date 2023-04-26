/*================================================================
*  
*  文件名称：connection.cpp
*  创 建 者: mongia
*  创建日期：2023年04月23日
*  邮    箱：mongiaK@outlook.com
*  
================================================================*/

#include "connection.h"

MConnection::MConnection(int32_t& fd, std::string& client_ip, long& client_port) 
    :_fd(fd)
     , _client_ip(client_ip)
     , _client_port(client_port)
     , _magic(0)
     , _magic_remain(4)
     , _parse_type(M_MAGIC)
{
}

MConnection::~MConnection() {

}
