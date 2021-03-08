/*================================================================
*  
*  文件名称：queue.h
*  创 建 者: mongia
*  创建日期：2020年08月01日
*  
================================================================*/

#pragma once

#include <pthread.h>
#include <stdint.h>
#include <list>

#include "lock.h"

template <class T>
class MQueue {
private:
    pthread_cond_t _cond;
    pthread_mutex_t _mutex;
    std::list<T>     _list;
public:
    MQueue() {
        pthread_cond_init(&_cond, NULL);
        pthread_mutex_init(&_mutex, NULL);
    }
    ~MQueue() {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }
public:
    int32_t push_msg(T& msg) {
        MutexLock lock(_mutex);

        _list.push_back(msg);

        // pthread_cond_signal(&_cond);

        return 0;
    }

    int32_t get_msg(T& msg) {
        MutexLock lock(_mutex);

        if(_list.empty()) {
            return 1;
        }
        
        msg = _list.front();
        _list.pop_front();
    }
    
};
