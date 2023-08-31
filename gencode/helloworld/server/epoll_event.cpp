#include "epoll_event.h"
#include "util.h"
#include <cstring>
#include <errno.h>
#include <sys/epoll.h>

MEpollEvent::MEpollEvent()
    : _epollfd(-1)

{}

MEpollEvent::~MEpollEvent() {}

bool MEpollEvent::Create() {
    _epollfd = epoll_create(1024);
    if (_epollfd == -1) {
        mlog_error("create epoll failed" << strerror(errno));
        return false;
    }
    return true;
}

int MEpollEvent::EpollCtl(int type, int socket, struct epoll_event &ev) {
    return epoll_ctl(_epollfd, type, socket, &ev);
}

void MEpollEvent::EpollLoop() {
    struct epoll_event events[1024];
    int sock; // TODO listen socket data, need to get in
    while (true) {
        int fdset = epoll_wait(_epollfd, events, 1024, 100);
        for (int i = 0; i < fdset; i++) {
            if (events[i].data.fd == sock) {
                // TODO accept client
                struct epoll_event ev;
                if (EpollCtl(EPOLL_CTL_ADD, ev.data.fd, &ev)) {
                    mlog_info("epoll add client fd failed.");
                    close(ev.data.fd);
                    continue;
                }
            } else {
                // TODO read, write data to socket buffer
            }
        }
    }
}
