/*================================================================
*  
*  文件名称：parser.h
*  创 建 者: mongia
*  创建日期：2023年04月23日
*  邮    箱：mongiaK@outlook.com
*  
================================================================*/

#ifndef __PARSER_H__
#define __PARSER_H__

#include "buffer_manager.h"
#include "util.h"

class MParser {
  public:
    MParser(MConnectionSP conn);
    ~MParser();

    MConnectionSP GetConn() { return _conn; }

    MRequestSP SplitMessage(MBufferSP buf, int& length);

    void Reset(); 

  private:
    MConnectionSP _conn;
    int32_t _header_remain;
    int32_t _rpc_remain;

    MRequestSP _req;
};

#endif // __PARSER_H__
