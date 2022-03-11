/*================================================================
*  
*  文件名称：mg_config.h
*  创 建 者: mongia
*  创建日期：2022年02月11日
*  邮    箱：mr_pengmj@outlook.com
*  
================================================================*/
#ifdef __MG_CONFIG_INCLUDE_H__
#define __MG_CONFIG_INCLUDE_H__

#include <bits/stdc++.h>
#include <signal.h>
#include <time.h>

#ifdef __LINUX__
#include <sys/time.h>
#elif __APPLE__
#include <sys/time.h>
#elif _WIN32
#endif

typedef void mg_void_t;
typedef int mg_int_t;
typedef int32_t mg_int32_t;
typedef uint32_t mg_uint32_t;
typedef int64_t mg_int64_t;
typedef uint64_t mg_uint64_t;
typedef double mg_double_t;
typedef float mg_float_t;
typedef int8_t mg_int8_t;
typedef uint8_t mg_uint8_t;
typedef int16_t mg_int16_t;
typedef uint16_t mg_uint16_t;
typedef bool mg_bool_t;
typedef char mg_char_t;
typedef size_t mg_size_t;
typedef unsigned long long mg_time_t;
typedef struct tm mg_tm_t;

typedef int32_t mg_atomic_int_t;
typedef uint32_t mg_atomic_uint_t;
typedef volatile mg_atomic_uint_t mg_atomic_t;
#endif
