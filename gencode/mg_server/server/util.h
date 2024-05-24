/*================================================================
*  
*  文件名称：util.h
*  创 建 者: mongia
*  创建日期：2023年04月23日
*  邮    箱：mongiaK@outlook.com
*  
================================================================*/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <memory>
#include  <iostream>

#define MAX_CONN_BUF 1024
#define MAGIC "peng"

enum {
    M_MAGIC = 1,
    M_HEADER = 2,
    M_RPC_HEADER = 3,
    M_RPC_CONTENT = 4,
};

struct MLengthHeader {
    int rpc_header_length;
    int rpc_body_length;
};

class MParser;
typedef std::shared_ptr<MParser> MParserSP;

class MConnection;
typedef std::shared_ptr<MConnection> MConnectionSP;

class MRequest;
typedef std::shared_ptr<MRequest> MRequestSP;

#define mlog_info(x) std::cout << "[" << __FILE__ << "] [" << __func__ << " +" << __LINE__ << "] [info] " <<  x << std::endl;
#define mlog_error(x) std::cerr << "[" << __FILE__ << "] [" << __func__ << " +" << __LINE__ << "] [error]" << x << std::endl;

#endif // __UTIL_H__
