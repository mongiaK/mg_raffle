/*================================================================
*
*  文件名称：mg_time.h
*  创 建 者: mongia
*  创建日期：2022年01月25日
*  邮    箱：mr_pengmj@outlook.com
*
================================================================*/
#ifndef __MG_TIME_INCLUDE_H__
#define __MG_TIME_INCLUDE_H__

#include "mg_config.h"
#include "mg_core.h"

#define mg_gettimeofday(x) gettimeofday(x, nullptr)

// 平台
#ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__
    #include <sys/time.h>
#define MG_GET_UTIME()              \
    struct timeval tv;              \
    (void) gettimeofday(&tv, NULL)  \
    return (tv.tv_sec * 1000000LL + tv.tv_usec)
#elif __ANDROID__
    #include <sys/time.h>
    #define MG_GET_UTIME()              \
    struct timeval tv;              \
    (void) gettimeofday(&tv, NULL)  \
    return (tv.tv_sec * 1000000LL + tv.tv_usec)
#elif __linux__
    #include <sys/time.h>
    #define MG_GET_UTIME()              \
    struct timeval tv;              \
    (void) gettimeofday(&tv, NULL)  \
    return (tv.tv_sec * 1000000LL + tv.tv_usec)
#else
#   error "Unknown compiler"
#endif

// 编译器
static mg_inline mg_time_t mg_utime() {
    struct timeval tv;
    mg_gettimeofday(&tv);
    return (tv.tv_sec * 1000000LL + tv.tv_usec);
}

static mg_inline mg_time_t mg_mtime() {
    struct timeval tv;
    mg_gettimeofday(&tv);
    return (tv.tv_sec * 1000LL + tv.tv_usec);
}

static mg_inline mg_time_t mg_time() {
    return time(nullptr);
}

static mg_inline mg_char_t* mg_logtime() {
    static char str[32];

    mg_tm_t *tmp;
    struct timeval tv;

    mg_gettimeofday(&tv);
    tmp = localtime(&tv.tv_sec);

    sprintf(str, "[%02d/%02d/%d:%02d:%02d:%02d] ", tmp->tm_mday,
            tmp->tm_mon, 1900 + tmp->tm_year, tmp->tm_hour, tmp->tm_min,
            tmp->tm_sec);
    return str;
}
#endif
