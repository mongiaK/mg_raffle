/*================================================================
*
*  文件名称：connection_manager.cpp
*  创 建 者: mongia
*  创建日期：2023年04月23日
*  邮    箱：mongiaK@outlook.com
*
================================================================*/

#include "connection_manager.h"
#include "connection.h"

#include <unistd.h>
#include <sys/epoll.h>

MConnManager::MConnManager(int epfd) : _epfd(epfd) {}

MConnManager::~MConnManager() {}

void MConnManager::CreateConn(int32_t& fd, std::string &client_ip,
                              long& client_port) {
    if (_conns.find(fd) != _conns.end()) {
        mlog_info("client fd exit ,cant accept again");
        return;
    }
    MConnectionSP conn(new MConnection(fd, client_ip, client_port));
    _conns[fd] = conn;
}

MConnectionSP MConnManager::GetConn(int fd) {
    auto it = _conns.find(fd);
    if (it == _conns.end()) {
        return nullptr;
    }
    return it->second;
}

void MConnManager::CloseConn(int fd) {
    struct epoll_event ev;
    ev.data.fd = fd;
    epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &ev);

    auto it = _conns.find(fd);
    if (it != _conns.end()) {
        _conns.erase(fd);
    }

    close(fd);
}
