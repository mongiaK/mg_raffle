/*================================================================
*
*  文件名称：epoll_event.h
*  创 建 者: mongia
*  创建日期：2021年06月03日
*
================================================================*/

#pragma once

#include "event.h"
#include "log.h"
#include "ss.h"

class SEpollEvent : public SEvent {
   public:
    SEpollEvent(int epsize = 1024) : _epfd(-1), _epsize(epsize) {}
    ~SEpollEvent() {
        if (_epfd > 0) {
            close(_epfd);
        }
    }
    virtual bool init() {
        int epollfd = epoll_create(_epsize);
        if (epollfd < 0) {
            slog_error("epoll create failed. " << strerror(errno));
            return false;
        }
        _epfd = epollfd;

        return true;
    }

    // TODO this can be do better， modify this
    virtual bool add_event(SEventInfoSP event_info) {
        struct epoll_event event;
        event.events = event_info->_events;
        event.data.ptr = new SEventInfo(event_info->_fd, event_info->_events,
                                        event_info->_callback);

        int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, event_info->_fd, &event);
        if (ret < 0) {
            slog_error("add socket to epoll queue failed. fd: "
                       << event_info->_fd << strerror(errno));
            return false;
        }
        return true;
    }

    virtual bool remove_event(SEventInfoSP event_info) {
        struct epoll_event event;
        int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, event_info->_fd, &event);
        if (ret < 0) {
            slog_error("remove socket " << event_info->_fd
                       << " from epoll queue failed. " << strerror(errno));
            return false;
        }
        if (event.data.ptr != NULL) delete (SEventInfo*)event.data.ptr;

        return true;
    }

    void event_run() {
        struct epoll_event events[_epsize];  // this can be done in first init
        for (;;) {
            int fds = epoll_wait(_epfd, events, _epsize, -1);
            if (fds < 0) {
                switch (errno) {
                    case EBADF:
                        slog_error("bad epoll file descripter");
                        break;
                    case EFAULT:
                        slog_error("can't write events to user memory");
                        break;
                    case EINTR:
                        slog_error("interupt by signal");
                        break;
                    case EINVAL:
                        slog_error("maxevents <= 0 or epfd is not epoll fd");
                        break;
                }
            } else if (fds == 0) {
                slog_debug("nothing happen");
            } else {
                for (int i = 0; i < fds; i++) {
                    SEventInfo* info =
                        reinterpret_cast<SEventInfo*>(events[i].data.ptr);
                    info->_events =
                        events[i]
                            .events;  // 当前触发的事件返回上层去处理，覆盖掉初始设置的，初始设置使用一次就没用了
                    info->_callback(SEventInfoSP(info));
                }
            }
        }
    }

   private:
    int _epfd;
    int _epsize;
};

