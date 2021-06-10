/*================================================================
*
*  文件名称：ss.h
*  创 建 者: mongia
*  创建日期：2021年06月01日
*
================================================================*/

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>

struct SEventInfo;
typedef std::shared_ptr<SEventInfo> SEventInfoSP;

class SEvent;
typedef std::shared_ptr<SEvent> SEventSP;

class SEpollEvent;
typedef std::shared_ptr<SEpollEvent> SEpollEventSP;

class SListen;
typedef std::shared_ptr<SListen> SListenSP;

class STCPListen;
typedef std::shared_ptr<STCPListen> STCPListenSP;

class SConnection;
typedef std::shared_ptr<SConnection> SConnectionSP;

class SEpollConnection;
typedef std::shared_ptr<SEpollConnection> SEpollConnectionSP;

class SServer;
typedef std::shared_ptr<SServer> SServerSP;

class STCPServer;
typedef std::shared_ptr<STCPServer> STCPServerSP;

class SBuffer;
typedef std::shared_ptr<SBuffer> SBufferSP;

class SBufferPool;
typedef std::shared_ptr<SBufferPool> SBufferPoolSP;

class SBufferGuard;

class SConnectionStat;
typedef std::shared_ptr<SConnectionStat> SConnectionStatSP;

class SStream;
typedef std::shared_ptr<SStream> SStreamSP;

typedef std::function<void(SEventInfoSP)> EventCallback;
