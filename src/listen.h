/*================================================================
*
*  文件名称：listen.h
*  创 建 者: mongia
*  创建日期：2021年06月03日
*
================================================================*/

#pragma once

#include "ss.h"

class SListen {
   public:
    SListen() {}
    virtual ~SListen(){};
    virtual void register_accept_callback(EventCallback callback) = 0;
    virtual bool start_listen() = 0;
    virtual int get_socket() const = 0;
    virtual void register_listen_event() = 0;
};

