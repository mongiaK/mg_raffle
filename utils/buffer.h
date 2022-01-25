/*================================================================
*  
*  文件名称：buffer.h
*  创 建 者: mongia
*  创建日期：2020年07月02日
*  
================================================================*/

#pragma once

#include <vector>
#include <atomic>

#include <stdint.h>
#include <pthread.h>
#include <assert.h>

#include "lock.h"

namespace  mg {

const static uint32_t BUFFER_BLOCK_SIZE = 4 * 1024;
const static uint32_t LARGE_BLOCK_SIZE  = 1024;

class BufferPool {
private:
    std::vector<char *> _buffer_pool;
    
    std::atomic<size_t> _memory_usage;

    char * _cur_ptr;

    size_t _cur_remain;

    mg::SpinMutex _mtx;

public:
    BufferPool() : _memory_usage(0) {
        _cur_ptr = nullptr;
        _cur_remain = 0;
    }
private:
    ~BufferPool() {
        for(size_t i = 0; i < _buffer_pool.size(); ++i) {
            delete [] _buffer_pool[i];
        }
    }
private:
    char * allocate_new_block(size_t size) {
        char * result = new char[size];
        _buffer_pool.push_back(result);
        _memory_usage.fetch_add(size, std::memory_order_relaxed);
        return result;
    }

    char * allocate_fallback(size_t size) {
        if(size >= LARGE_BLOCK_SIZE) {
            return allocate_new_block(size);
        }

        _cur_ptr = allocate_new_block(BUFFER_BLOCK_SIZE);
        _cur_remain = BUFFER_BLOCK_SIZE;

        char * result = _cur_ptr;
        _cur_ptr += size;
        _cur_remain -= size;

        return result;
    }
public:
    char * allocate(size_t size) {
        assert(size > 0);

        SpinLock _(_mtx);
        if(_cur_remain > size) {
            char * result = _cur_ptr;
            _cur_ptr += size;
            _cur_remain -= size;
            return result;
        }

        return allocate_fallback(size);
    }

    char * allocate_align(size_t size) {
        assert(size > 0);

        SpinLock _(_mtx);

        uint32_t align = sizeof(void *);
        assert((align & (align - 1)) != 0);
        uint32_t mod = reinterpret_cast<uintptr_t>(_cur_ptr) & (align - 1);
        uint32_t slot = ((mod == 0) ? 0 : (align - mod));
        size_t needed = size + slot;
        if(needed > _cur_remain) {
            return allocate_fallback(size);
        }

        char * result = _cur_ptr + slot;
        _cur_ptr += needed;
        _cur_remain -= needed;
        return result;
    }

    size_t memory_usage() const {
        return _memory_usage.load(std::memory_order_relaxed);
    }
};

}
