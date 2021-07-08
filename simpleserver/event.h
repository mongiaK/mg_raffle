/*================================================================
*  
*  文件名称：event.h
*  创 建 者: mongia
*  创建日期：2021年06月03日
*  
================================================================*/

#pragma once

#include "ss.h"

struct SEventInfo {
    int _fd;
    int _events;
    EventCallback _callback;
    SEventInfo(int fd, int events, EventCallback callback)
        : _fd(fd), _events(events), _callback(callback) {}
};
typedef std::shared_ptr<SEventInfo> SEventInfoSP;

class SEvent {
   public:
       SEvent() {}
    virtual ~SEvent() {};
    virtual bool init() = 0;
    virtual void event_run() = 0;
    virtual bool add_event(SEventInfoSP) = 0;
    virtual bool remove_event(SEventInfoSP) = 0;
};


