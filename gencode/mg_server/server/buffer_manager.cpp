/*================================================================
*
*  文件名称：buffer_manager.cpp
*  创 建 者: mongia
*  创建日期：2023年04月26日
*  邮    箱：mongiaK@outlook.com
*
================================================================*/

#include "buffer_manager.h"

MBufferGuard::MBufferGuard(MBufferSP buf, int32_t size)
    : _buf(buf), _buf_free(false), _size(size), _index(_buf->Buf()) {}
MBufferGuard::~MBufferGuard() {
    if (false == _buf_free) {
        MBufferPool::Instance()->GiveBack(_buf);
        _buf_free = true;
    }
}

char* MBufferGuard::Buf() { return _index; }

int32_t MBufferGuard::Size() { return _size; }

