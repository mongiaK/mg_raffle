/*================================================================
*
*  文件名称：connection.h
*  创 建 者: mongia
*  创建日期：2021年06月03日
*
================================================================*/

#pragma once

#include "ss.h"

class SConnection {
   public:
    SConnection() {}
    virtual ~SConnection() {}
    virtual void set_dst_ip(char* dst_ip) = 0;
    virtual void set_dst_port(int dst_port) = 0;
    virtual std::string get_dst_ip() const = 0;
    virtual int get_dst_port() const = 0;
    virtual int get_socket() const = 0;
    virtual bool register_event() = 0;
    virtual bool remove_event() = 0;
    virtual void on_close() = 0;
};

class SConnectionStat {
   public:
    SConnectionStat() : _last_tick_time(0), _recieve_data_size(0) {}
    int _last_tick_time;
    int64_t _recieve_data_size;
};

