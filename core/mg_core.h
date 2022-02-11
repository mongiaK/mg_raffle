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
#include <time.h>
#include <signal.h>

#ifdef __LINUX__
#include <sys/time.h>
#elif __APPLE__
#include <sys/time.h>
#elif _WIN32
#endif

typedef struct mg_list _mg_list_t;
typedef struct mg_skiplist _mg_skiplist_t;
typedef struct mg_pool _mg_pool_t;
typedef struct mg_rb_node _mg_rb_node_t;
typedef struct mg_rb_tree _mg_rb_tree_t;
typedef struct mg_buf _mg_buf_t;

typedef void _mg_void_t;
typedef int _mg_int_t;
typedef int32_t _mg_int32_t;
typedef uint32_t _mg_uint32_t;
typedef int64_t _mg_int64_t;
typedef uint64_t _mg_uint64_t;
typedef double _mg_double_t;
typedef float _mg_float_t;
typedef int8_t _mg_int8_t;
typedef uint8_t _mg_uint8_t;
typedef int16_t _mg_int16_t;
typedef uint16_t _mg_uint16_t;
typedef bool _mg_bool_t;
typedef char _mg_char_t;
typedef size_t _mg_size_t;
typedef unsigned long long _mg_time_t;
typedef struct tm _mg_tm_t;


#define _mg_inline inline

#define _mg_assert(x) assert(x)
#define _mg_posix_memalign posix_memalign
#define _mg_malloc malloc
#define _mg_free free
#define _mg_mprotect mprotect
#ifndef offsetof
#define offsetof(x, member) ((_mg_uint32_t)&(((x*)0)->member))
#endif

#define _MG_STRUCT_POINT(s, p, member)   \
    ((s*)((_mg_char_t*)(p)-offsetof(s, member)))

#ifndef nullptr
#define nullptr NULL
#endif

#define _mg_gettimeofday(x) gettimeofday(x, nullptr)

// return
// = 0: l = r
// > 0: l < r
// < 0: l > r
typedef _mg_int32_t (*_mg_rb_node_compare)(_mg_rb_node_t* l, _mg_rb_node_t* r);
typedef _mg_int32_t (*_mg_skiplist_node_compare)(_mg_list_t* l, _mg_list_t* r);

#include "mg_platform.h"
#include "mg_list.h"
#include "mg_rbtree.h"
#include "mg_pool.h"
#include "mg_time.h"

#endif
