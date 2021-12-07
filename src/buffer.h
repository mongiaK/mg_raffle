/*================================================================
*
*  文件名称：buffer.h
*  创 建 者: mongia
*  创建日期：2021年06月04日
*
================================================================*/

#pragma once

#include <assert.h>
#include <string.h>

#include <cstdlib>
#include <list>
#include <map>
#include <mutex>
#include <queue>

#include "log.h"
#include "ss.h"

static int malloc_size(int size) {
    assert(size <= 4096);
    if (size > 2048 && size <= 4096) {
        return 4096;
    } else if (size > 1024 && size <= 2048) {
        return 2048;
    } else if (size > 512 && size <= 1024) {
        return 1024;
    } else if (size > 256 && size <= 512) {
        return 512;
    } else if (size > 128 && size <= 256) {
        return 256;
    } else {
        return 128;
    }
}

class SBuffer {
   public:
    SBuffer(int allocate_size) : _alloc_size(allocate_size) {
        _real_size = malloc_size(_alloc_size);
        _headp = (char*)malloc(1000 * sizeof(char));
        if (_headp == NULL) {
            slog_warn("new buffer fail" << strerror(errno));
            _real_size = 0;
        }
        _remainp = _headp;
        _remain_size = _real_size;
    }

    ~SBuffer() {
        if (_headp != NULL) {
            free(_headp);
            _headp = NULL;
        }
    }

    void usage(int len) {
        assert(len <= _remain_size);
        _remainp += len;
        _remain_size -= len;
    }

    void reset() {
        _remainp = _headp;
        _remain_size = _real_size;
    }

    char* get_buf() const { return _remainp; }
    int get_remain_size() const { return _remain_size; }

    int get_size() const { return _alloc_size; }
    int get_real_size() const { return _real_size; }
    int get_usage_size() const { return _remainp - _headp; }

   private:
    int _alloc_size;
    int _real_size;
    char* _headp;
    char* _remainp;
    int _remain_size;
};

class SBufferPool {
    class BufferList {
       public:
        BufferList(int size) : _buffer_size(size) {
            for (int i = 0; i < 1000; i++) {
                SBufferSP buf(new SBuffer(size));
                if (buf->get_real_size() > 0) {
                    _buf_list.push_back(buf);
                }
            }
        }
        ~BufferList() {}
        int _buffer_size;
        std::mutex _mtx;
        std::list<SBufferSP> _buf_list;

        void give_back(SBufferSP buf) {
            std::unique_lock<std::mutex> _(_mtx);
            _buf_list.push_back(buf);
        }

        SBufferSP get_buf() {
            std::unique_lock<std::mutex> _(_mtx);
            if (_buf_list.empty()) {
                return SBufferSP(new SBuffer(_buffer_size));
            }
            SBufferSP buf = _buf_list.front();
            _buf_list.pop_front();
            return buf;
        }
    };
    typedef std::shared_ptr<BufferList> BufferListSP;

   public:
    SBufferPool() {
        _bufsets[128] = BufferListSP(new BufferList(128));
        _bufsets[256] = BufferListSP(new BufferList(256));
        _bufsets[512] = BufferListSP(new BufferList(512));
        _bufsets[1024] = BufferListSP(new BufferList(1024));
        _bufsets[2048] = BufferListSP(new BufferList(2048));
        _bufsets[4096] = BufferListSP(new BufferList(4096));
    }
    ~SBufferPool(){};

    SBufferSP get_buffer(int size) {
        int real_size = malloc_size(size);
        BufferListSP& li = _bufsets[real_size];
        return li->get_buf();
    }

    void give_back(SBufferSP buf) {
        int real_size = buf->get_real_size();
        BufferListSP& li = _bufsets[real_size];
        li->give_back(buf);
    }

   private:
    std::map<int, BufferListSP> _bufsets;  // 按块大小分类
};

class SBufferGuard {
   public:
    SBufferGuard(SBufferPoolSP poolsp, int size)
        : _buf_size(size), _buf_poolsp(poolsp) {
        _bufsp = _buf_poolsp->get_buffer(_buf_size);
    }
    ~SBufferGuard() { _buf_poolsp->give_back(_bufsp); }

    SBufferSP operator->() { return _bufsp; }

   private:
    SBufferSP _bufsp;
    int _buf_size;
    SBufferPoolSP _buf_poolsp;
};
typedef std::shared_ptr<SBufferGuard> SBufferGuardSP;

struct BufHandle {
    BufHandle(int offset, int size, SBufferGuardSP bufsp)
        : _offset(offset), _size(size), _bufsp(bufsp) {}
    int _offset;
    int _size;
    SBufferGuardSP _bufsp;
};

class ReadBuffer {
   public:
    void Append(const BufHandle& buf_handle) {
        _buf_list.push_back(buf_handle);
    }

   private:
    std::deque<BufHandle> _buf_list;
};
typedef std::shared_ptr<ReadBuffer> ReadBufferSP;

struct RecieveItem {
    RecieveItem(int meta_size, int data_size, ReadBufferSP read_bufsp)
        : _meta_size(meta_size),
          _data_size(data_size),
          _read_bufsp(read_bufsp) {}
    int _meta_size;
    int _data_size;
    ReadBufferSP _read_bufsp;
};

class WriteBuffer {};
typedef std::shared_ptr<WriteBuffer> WriteBufferSP;
