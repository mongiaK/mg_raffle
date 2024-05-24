/*================================================================
*
*  文件名称：connection.cpp
*  创 建 者: mongia
*  创建日期：2023年04月23日
*  邮    箱：mongiaK@outlook.com
*
================================================================*/

#include "connection.h"

#include <sys/socket.h>

#include "buffer_manager.h"
#include "connection_manager.h"

extern MConnManagerSP gConnManager;

MConnection::MConnection(int32_t& fd, std::string& client_ip, long& client_port)
    : _fd(fd),
      _client_ip(client_ip),
      _client_port(client_port),
      _magic(0),
      _magic_remain(4),
      _parse_type(M_MAGIC),
      _send_buf(new MReadBuf()) {}

MConnection::~MConnection() {}

void MConnection::SendBuf() {
    const void* data = nullptr;
    int size = 0;
    int ret = 0;
    while (_send_buf->Next(&data, &size)) {
        ret = send(_fd, data, size, 0);
        if (ret == 0) {
            mlog_info("conn close");
            gConnManager->CloseConn(_fd);
            return;
        } else if (ret < 0) {
            switch (errno) {
                case EAGAIN | EWOULDBLOCK | EINTR:
                    _send_buf->BackUp(size);
                    break;
                default:
                    gConnManager->CloseConn(_fd);
                    return;
            }
        } else {
            if(ret < size) {
                _send_buf->BackUp(size - ret);
            } else {
                _send_buf->Erase();
            }
        }
    }
}
