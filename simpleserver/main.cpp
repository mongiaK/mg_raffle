/*================================================================
*  
*  文件名称：main.cpp
*  创 建 者: mongia
*  创建日期：2021年06月10日
*  
================================================================*/

#include "ss.h"
#include "common.h"
#include "tcp_server.h"

int main(int argc, char* argv[]) {
    if(is_little_endian()) {
        std::cerr << "is not little endian" << std::endl;
        return 1;
    }

    STCPServer server;
    if (!server.init()) {
        std::cerr << "server init failed" << std::endl;
        return 1;
    }

    server.start();

    return 0;
}
