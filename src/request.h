/*================================================================
*  
*  文件名称：request.h
*  创 建 者: mongia
*  创建日期：2021年06月30日
*  
================================================================*/

#pragma once

#include "buffer.h"
#include <cstdint>
#include <memory>
class SRequest {
    public:
        SRequest(){}
        virtual ~SRequest() {}
        virtual ReadBufferSP get_read_buf() = 0;
        virtual void process_request() = 0;

        virtual void set_recieve_time(int64_t ) = 0;
        virtual void set_remote_ip(const std::string&) = 0;
        virtual void set_remote_port(const int32_t) = 0;
};

typedef std::shared_ptr<SRequest> SRequestSP;
