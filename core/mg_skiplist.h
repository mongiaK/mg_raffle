/*================================================================
*
*  文件名称：mg_skiplist.h
*  创 建 者: mongia
*  创建日期：2022年01月25日
*  邮    箱：mr_pengmj@outlook.com
*
================================================================*/
#ifndef __MG_SKIPLIST_INCLUDE_H__
#define __MG_SKIPLIST_INCLUDE_H__

#include "mg_config.h"
#include "mg_core.h"

#define MAX_SKIPLIST_LEVEL 64

// return
// = 0: l = r
// > 0: l < r
// < 0: l > r
typedef mg_int32_t (*mg_skiplist_node_compare)(mg_list_t* l, mg_list_t* r);

typedef struct mg_skiplist_node {
    mg_uint32_t _level;
    mg_list_t* _link;
} mg_skiplist_node_t;

typedef struct mg_skiplist {
    mg_uint32_t _cur_level;
    mg_skiplist_node_compare _compare;
    mg_list_t* _head[MAX_SKIPLIST_LEVEL];
} mg_skiplist_t;

static mg_inline mg_void_t init_skiplist(mg_skiplist* skl,
                                         mg_skiplist_node_compare compare) {
    for (mg_int32_t i = 0; i < MAX_SKIPLIST_LEVEL; i++) {
        MG_LIST_INIT(skl->_head[i]);
    }
    skl->_cur_level = 0;
    skl->_compare = compare;
}

#endif
