/*================================================================
*
*  文件名称：spin_lock.h
*  创 建 者: mongia
*  创建日期：2020年08月01日
*
================================================================*/

#pragma once

#include <atomic>

namespace mg {

class SpinMutex {
   public:
    SpinMutex() : _mutex(ATOMIC_FLAG_INIT) {}
    ~SpinMutex() {}
    SpinMutex(const SpinMutex&) = delete;
    SpinMutex& operator=(const SpinMutex&) = delete;

    void lock() {
        while (_mutex.test_and_set())
            ;
    }

    void unlock() { _mutex.clear(); }

   private:
    std::atomic_flag _mutex;
};

class SpinLock {
   private:
    SpinMutex& _mtx;

   public:
    explicit SpinLock(SpinMutex& mtx) : _mtx(mtx) { _mtx.lock(); }

    ~SpinLock() { _mtx.unlock(); }

    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
};

}  // namespace mg

#ifdef MG_TEST
#include <thread>
#include <vector>
#include <function>

#include "log.h"

SpinMutex mtx;

void append(int x) {
    SpinLock _(mtx);
    plog_info("thread #" << x);
}

void lock_test() {
    std::vector<std::thread> threads;
    for(int i = 0; i < 10; i++) {
        threads.push_back(std::thread(append, i);)
    }
    for(auto& t: threads) t.join();
}
#endif
