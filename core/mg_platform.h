/*================================================================
*  
*  文件名称：mg_platform.h
*  创 建 者: mongia
*  创建日期：2022年02月11日
*  邮    箱：mr_pengmj@outlook.com
*  
================================================================*/

#ifndef __MG_PLATFORM_INCLUDE_H__
#define __MG_PLATFORM_INCLUDE_H__

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

#endif
