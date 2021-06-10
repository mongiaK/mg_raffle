/*================================================================
*
*  文件名称：server.h
*  创 建 者: mongia
*  创建日期：2021年06月04日
*
================================================================*/

#pragma once

#include "ss.h"

class SServer {
   public:
    SServer() {}
    virtual ~SServer(){};
    virtual bool init() = 0;
    virtual bool start() = 0;
    virtual SBufferPoolSP get_buf_pool() = 0;
    virtual void release_connection(SConnectionSP) = 0;
    virtual SConnectionSP create_connection(int, SEventSP) = 0;
};
