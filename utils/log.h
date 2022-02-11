/*================================================================
*
*  文件名称：log.h
*  创 建 者: mongia
*  创建日期：2021年04月01日
*
================================================================*/

#include <iostream>

#define plog_debug(x) std::cout << "[debug] " << x << std::endl;
#define plog_info(x) std::cout << "[info ] " << x << std::endl;
#define plog_warn(x) std::cout << "[warn ] " << x << std::endl;
#define plog_error(x) std::cout << "[error] " << x << std::endl;
#define plog_fatal(x)                              \
    {                                              \
        std::cout << "[fatal] " << x << std::endl; \
        exit(1);                                   \
    }

enum LogLevel {
    DEBUG = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4,
    FATAL = 5,
};

FILE* g_fp = stdout;
LogLevel g_level = DEBUG;
char g_filename[1024];

bool plog_init(const char* filename, LogLevel level = DEBUG) {
}
