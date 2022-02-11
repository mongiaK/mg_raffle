/*================================================================
*
*  文件名称：mg_skiplist.h
*  创 建 者: mongia
*  创建日期：2022年01月25日
*  邮    箱：mr_pengmj@outlook.com
*
================================================================*/

#pragma once

#include "mg_core.h"
#include "mg_list.h"

#define MAX_SKIPLIST_LEVEL  64

struct mg_skiplist_node {
    _mg_uint32_t _level;
    _mg_list_t* _link;
};

struct mg_skiplist {
    _mg_uint32_t _cur_level; 
    _mg_skiplist_node_compare _compare;
    _mg_list_t* _head[MAX_SKIPLIST_LEVEL];
};

static inline _mg_void_t init_skiplist(mg_skiplist* skl, _mg_skiplist_node_compare compare) {
    for(_mg_int32_t i = 0; i < MAX_SKIPLIST_LEVEL; i++) {
        _MG_LIST_INIT(skl->_head[i]);
    }
    skl->_cur_level = 0;
    skl->_compare = compare;
}

