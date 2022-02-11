/*================================================================
*
*  文件名称：decorator_mode.h
*  创 建 者: mongia
*  创建日期：2021年01月19日
*  邮    箱：mr_pengmj@outlook.com
*
================================================================*/

#pragma once

#include <iostream>
#include <memory>

// 设计模式：装饰模式
// 设计实现：通常多层继承的时候使用，在冗余代码比较多的情况使用
//
// 使用场景：单个基类，多个子类的情况,且至少有3层继承关系的情况
namespace DesignMode {

class Stream {
   public:
    Stream();
    virtual ~Stream();

   public:
    virtual int read();
    virtual int write();
};

class ReadStream : public Stream {
    public:
        ReadStream();
        virtual ~ReadStream();

    public:
        virtual int read();
        virtual int write();
};

class BytesStream {
   protected:
    std::shared_ptr<Stream> stream;

   public:
    BytesStream(std::shared_ptr<Stream> stream);
    virtual ~BytesStream();
};

class RpcBytesStream : public BytesStream {
   public:
    RpcBytesStream(std::shared_ptr<Stream> stream) : BytesStream(stream){};
    virtual ~RpcBytesStream();

   public:
    int read() {
        stream->read();
        return 0;
    }

    int write() {
        stream->write();
        return 0;
    }
};

static void decorator_mode() {
    std::shared_ptr<ReadStream> rd(new ReadStream());
    std::shared_ptr<RpcBytesStream> rpc_bytes(new RpcBytesStream(rd));
}

}  // namespace DesignMode
