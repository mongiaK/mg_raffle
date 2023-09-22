/*================================================================
*  
*  文件名称：net_header.h
*  创 建 者: mongia
*  创建日期：2023年09月22日
*  邮    箱：mongiaK@outlook.com
*  
================================================================*/

#ifndef __NET_HEADER_H__
#define __NET_HEADER_H__

#include <stdio.h>      // 标准输入输出头文件，用于打印调试信息
#include <stdlib.h>     // 标准库头文件，包含内存分配和释放函数
#include <string.h>     // 字符串操作头文件，用于处理数据缓冲区
#include <errno.h>      // 错误处理头文件，包含错误码和错误信息
#include <unistd.h>     // Unix标准头文件，包含系统调用函数，如close()、read()、write()等
#include <sys/socket.h> // Socket编程头文件，包含Socket相关函数和数据结构的定义
#include <netinet/in.h> // Internet地址头文件，包含处理IPv4地址的函数和数据结构的定义
#include <arpa/inet.h>  // Internet地址转换头文件，包含IP地址转换函数
#include <netdb.h>      // 网络数据库头文件，用于域名解析和网络服务查询的函数
#include <fcntl.h>      // 文件控制头文件，用于设置非阻塞Socket
#include <pthread.h>    // 线程头文件，如果需要多线程支持
#include <signal.h>     // 信号处理头文件，用于处理进程间通信和错误处理

#endif // __NET_HEADER_H__
       //
