#ifndef __epoll_event_h__
#define __epoll_event_h__

#include <functional>
#include <sys/epoll.h>

class MEpollEvent {
  public:
    MEpollEvent();
    ~MEpollEvent();

    static MEpollEvent &Instance() {
        static MEpollEvent ins;
        return ins;
    }

    int32_t EpollFD() const { return _epollfd; }

    bool Create();

    int EpollCtl(int type, int socket, struct epoll_event &ev);

    void EpollLoop();

  private:
    int32_t _epollfd;
    struct epoll_event events[1024];
};

typedef std::function<int(int, int, struct epoll_event &)> epoll_ctrl_callback;

#endif
