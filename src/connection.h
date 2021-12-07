/*================================================================
*
*  文件名称：connection.h
*  创 建 者: mongia
*  创建日期：2021年06月03日
*
================================================================*/

#pragma once

#include "ss.h"
#include <cstdint>

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
    virtual void fresh_stat(int size) = 0;
    virtual void register_request_callback(RequestCallback) = 0;
};

class SConnectionStat {
   public:
    SConnectionStat() : _last_tick_time(0), _recieve_data_size(0) {}
    ~SConnectionStat() {}
    void refresh_stat(int size) {
        _last_tick_time = time(NULL);
        _recieve_data_size += size;
    }
    std::atomic<uint64_t> _last_tick_time;
    std::atomic<uint64_t> _recieve_data_size;
};

