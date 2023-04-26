/*================================================================
*
*  文件名称：buffer_manager.h
*  创 建 者: mongia
*  创建日期：2023年04月23日
*  邮    箱：mongiaK@outlook.com
*
================================================================*/

#ifndef __BUFFER_MANAGER_H__
#define __BUFFER_MANAGER_H__

#include <google/protobuf/io/zero_copy_stream.h>

#include <list>
#include <memory>

#include "util.h"

#define BUFFER_POOL_SIZE 1024

class MBuffer {
   public:
    MBuffer(int32_t capacity) : _cur_index(0) {
        _buf = new char[capacity];
        if (_buf == nullptr) {
            _capacity = 0;
        } else {
            _capacity = capacity;
        }
        _remain = _capacity;
    }

    ~MBuffer() {
        mlog_info("mbuffer free");
        delete _buf;
    }

    char* Buf() const { return _buf + _cur_index; }

    int32_t Remain() const { return _remain; }

    void Consume(int32_t offset) {
        _cur_index += offset;
    }

    bool Empty() { return (_remain == 0); }

    int32_t Capacity() const { return _capacity; }

    void Reset() {
        _cur_index = 0;
        _remain = _capacity;
    }

   private:
    char* _buf;
    int32_t _capacity;

    int32_t _cur_index;
    int32_t _remain;
};

typedef std::shared_ptr<MBuffer> MBufferSP;

class MBufferPool {
   public:
    MBufferPool() {
        for (int i = 0; i < BUFFER_POOL_SIZE; i++) {
            MBufferSP buf(new MBuffer(1024));
            if (buf->Capacity() > 0) _pool1024.push_back(buf);
        }
    }

    ~MBufferPool() {}

    MBufferSP Allocate() {
        if (_pool1024.empty()) {
            return nullptr;
        }
        MBufferSP buf = _pool1024.front();
        _pool1024.pop_front();

        return buf;
    }

    void GiveBack(MBufferSP buf) { _pool1024.push_back(buf); }

   private:
    std::list<MBufferSP> _pool1024;
};

typedef std::shared_ptr<MBufferPool> MBufferPoolSP;

class MBufferGuard {
   public:
    MBufferGuard(MBufferPoolSP pool, MBufferSP buf, int32_t size);
    ~MBufferGuard();
    char* Buf();

    int32_t Size();

   private:
    MBufferPoolSP _pool;
    MBufferSP _buf;
    bool _buf_free;

    int32_t _size;
    char* _index;
};

typedef std::shared_ptr<MBufferGuard> MBufferGuardSP;

class MReadBuf : public google::protobuf::io::ZeroCopyInputStream {
   public:
    MReadBuf() : _cur_it(_buf_list.begin()), _cur_pos(0), _read_bytes(0), _total_bytes(0) {}
    void Append(MBufferGuardSP bufguard) { 
        _buf_list.push_back(bufguard); 
        _cur_it = _buf_list.begin();
        _total_bytes += bufguard->Size();
    }

    bool Next(const void** data, int* size) {
        if(_cur_it != _buf_list.end()) {
            *data = _cur_it->get()->Buf() + _cur_pos;
            *size = _cur_it->get()->Size() - _cur_pos;
            _cur_it++;
            _read_bytes += *size;
            return true;
        }

        return false;
    }

    void BackUp(int count) {
        if(count > 0) {
            --_cur_it;
            _cur_pos = _cur_it->get()->Size() - count;
        }
        _read_bytes -= count;
    }

    int32_t TotalBytes() const {
        return _total_bytes;
    }

    bool Skip(int count) {
        const void* data;
        int size;
        while(count > 0 && Next(&data, &size)) {
            if(size > count) {
                BackUp(size - count);
                size = count;
            }
            count -= size;
        }
        return count == 0;
    }

    int64_t ByteCount() const {
        return _read_bytes;
    }

   private:
    std::list<MBufferGuardSP> _buf_list;
    std::list<MBufferGuardSP>::iterator _cur_it;

    int32_t _cur_pos;
    int32_t _read_bytes;
    int32_t _total_bytes;
};

typedef std::shared_ptr<MReadBuf> MReadBufSP;

class MWriteBuf : public google::protobuf::io::ZeroCopyOutputStream {
    public:

};

#endif  // __BUFFER_MANAGER_H__
