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

#define mg_inline inline
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

#include "mg_list.h"
#include "mg_pool.h"
#include "mg_rbtree.h"
#include "mg_time.h"
#include "mg_log.h"

#endif
