/*================================================================
*  
*  文件名称：spin_lock.h
*  创 建 者: mongia
*  创建日期：2020年08月01日
*  
================================================================*/

#pragma once

#include <pthread.h>

class SpinLock {
private:
    pthread_spinlock_t& _lock;
public:
    explicit SpinLock(pthread_spinlock_t& lock) : _lock(lock) {
        pthread_spin_lock(&_lock);
    }

    ~SpinLock() {
        pthread_spin_unlock(&_lock);    
    }

    SpinLock(const SpinLock& ) = delete;
    SpinLock& operator =(const SpinLock& ) = delete;
};

class MutexLock {
private:
    pthread_mutex_t& _lock;
public:
    explicit MutexLock(pthread_mutex_t& lock): _lock(lock) {
        pthread_mutex_lock(&_lock);
    }

    ~MutexLock() {
        pthread_mutex_unlock(&_lock);
    }

    MutexLock(const MutexLock&) = delete;
    MutexLock& operator =(const MutexLock&) = delete;
};
