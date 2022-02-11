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

struct mg_list {
    struct mg_list* _prev;
    struct mg_list* _next;
};

static _mg_inline _mg_void_t init_list(mg_list* l) {
    l->_next = l;
    l->_prev = l;
}

static _mg_inline _mg_void_t insert_list_front(mg_list* e, mg_list* l) {
    e->_next = l;
    e->_prev = l->_prev;
    l->_prev->_next = e;
    l->_prev = e;
}

static _mg_inline _mg_void_t insert_list_end(mg_list* e, mg_list* l) {
    e->_next = l->_next;
    e->_prev = l;
    l->_next->_prev = e;
    l->_next = e;
}

static _mg_inline _mg_void_t remove_list_node(mg_list* e, _mg_bool_t cleanup) {
    e->_prev->_next = e->_next;
    e->_next->_prev = e->_prev;

    if(cleanup) {
        init_list(e);
    }
}

static _mg_inline mg_list* list_head(mg_list* l) {
    return l->_next;
}

static _mg_inline mg_list* list_tail(mg_list* l) {
    return l->_prev;
}

#define _MG_LIST_INIT(l) init_list(l)
#define _MG_LIST_EMPTY(l) ((l) == (l)->_prev)
#define _MG_LIST_PUSH_BACK(e, l) insert_list_end(e, l)
#define _MG_LIST_PUSH_FRONT(e, l) insert_list_front(e, l)
#define _MG_LIST_DELETE(e, cleanup) remove_list_node(e, cleanup)
#define _MG_LIST_HEAD(l) list_head(l)
#define _MG_LIST_TAIL list_tail(l)
#define _MG_LIST_STATIC_INIT(l) \
    {(l), (l)}

#endif
