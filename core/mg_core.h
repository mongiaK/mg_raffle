/*================================================================
*
*  文件名称：mg_core.h
*  创 建 者: mongia
*  创建日期：2022年01月25日
*  邮    箱：mr_pengmj@outlook.com
*
================================================================*/
#ifndef __MG_CORE_INCLUDE_H__
#define __MG_CORE_INCLUDE_H__

#include <bits/stdc++.h>
#include <signal.h>
#include <time.h>

#ifdef __LINUX__
#include <sys/time.h>
#elif __APPLE__
#include <sys/time.h>
#elif _WIN32
#endif

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

#define mg_assert(x) assert(x)
#define mg_posix_memalign posix_memalign
#define mg_malloc malloc
#define mg_free free
#define mg_mprotect mprotect

#ifndef offsetof
#define offsetof(x, member) ((mg_uint32_t) & (((x*)0)->member))
#endif

#ifndef MG_STRUCT_POINT
#define MG_STRUCT_POINT(s, p, member) \
    ((s*)((mg_char_t*)(p)-offsetof(s, member)))
#endif

#ifndef nullptr
#define nullptr NULL
#endif


#endif
