/*================================================================
*  
*  文件名称：common.h
*  创 建 者: mongia
*  创建日期：2021年06月03日
*  
================================================================*/
#pragma once

#include <fcntl.h>

static void setnonblock(int fd) {
    auto stat = fcntl(fd, F_GETFL, 0);
    stat |= O_NONBLOCK;
    fcntl(fd, F_SETFL, stat);
}

static bool is_little_endian() {
    union endian {
        int32_t a;
        char  b;
    } ;
    static endian val = {1};
    return val.b != 1;
}
