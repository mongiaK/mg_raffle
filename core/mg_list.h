/*================================================================
*
*  文件名称：mg_list.h
*  创 建 者: mongia
*  创建日期：2022年01月27日
*  邮    箱：mr_pengmj@outlook.com
*
================================================================*/

#ifndef __MG_LIST_INCLUDE_H__
#define __MG_LIST_INCLUDE_H__

#include "mg_core.h"

typedef struct mg_list {
    struct mg_list* _prev;
    struct mg_list* _next;
} mg_list_t;

static inline void init_list(mg_list* l);

static inline void insert_list_front(mg_list* e, mg_list* l);

static inline void insert_list_end(mg_list* e, mg_list* l);

static inline void remove_list_node(mg_list* e, mg_bool_t cleanup);

static inline mg_list* list_head(mg_list* l);

static inline mg_list* list_tail(mg_list* l);

#define MG_LIST_INIT(l) init_list(l)
#define MG_LIST_EMPTY(l) ((l) == (l)->_prev)
#define MG_LIST_PUSH_BACK(e, l) insert_list_end(e, l)
#define MG_LIST_PUSH_FRONT(e, l) insert_list_front(e, l)
#define MG_LIST_DELETE(e, cleanup) remove_list_node(e, cleanup)
#define MG_LIST_HEAD(l) list_head(l)
#define MG_LIST_TAIL list_tail(l)
#define MG_LIST_STATIC_INIT(l) \
    { (l), (l) }

#endif
