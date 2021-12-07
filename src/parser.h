/*================================================================
*
*  文件名称：parser.h
*  创 建 者: mongia
*  创建日期：2021年06月30日
*
================================================================*/

#pragma once

#include "buffer.h"
#include "request.h"
#include <queue>
#include <memory>
class SParser {
   public:
    SParser() {}
    virtual ~SParser() {}
    virtual int parser_msg(SBufferGuardSP bufsp, int& consume, int& remain_length) = 0;
    virtual SRequestSP get_request() = 0;
};

typedef std::shared_ptr<SParser> SParserSP;
