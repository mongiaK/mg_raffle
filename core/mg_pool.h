/*================================================================
*
*  文件名称：mg_pool.h
*  创 建 者: mongia
*  创建日期：2022年01月27日
*  邮    箱：mr_pengmj@outlook.com
*
================================================================*/
#ifndef __MG_POOL_INCLUDE_H__
#define __MG_POOL_INCLUDE_H__

#include "mg_core.h"
#include "mg_list.h"

#ifndef MG_PAGESIZE
#define MG_PAGESIZE (4 * 1024)
#endif

#ifndef MG_ALIGNEMNT
#define MG_ALIGNMENT (32)
#endif

#ifdef MG_DEBUG
#define MG_REDZONE MG_PAGESIZE
#else
#define MG_REDZONE (0)
#endif

#ifndef MG_MAX_ALLOC_FROM_POOL
#define MG_MAX_ALLOC_FROM_POOL (MG_PAGESIZE - 1 - sizeof(mg_small_block))
#endif

#define MG_ALIGN(n, alignment) (((n) + (alignment - 1)) & ~(alignment - 1))

struct mg_large_block {
    mg_list_t _l;
    void* _head;
    void* _buf;
};

struct mg_small_block {
    mg_list_t _l;
    void* _head;
    mg_char_t* _last;
    mg_char_t* _end;
    mg_int_t _failed;
};

typedef struct mg_pool {
    mg_size_t _max;
    mg_list_t* _current;
    mg_list_t _large;
    mg_list_t _small;
} mg_pool_t;

mg_pool* mg_create_pool(mg_size_t size) {
    mg_pool* p;
    p = (mg_pool*)mg_malloc(sizeof(mg_pool));
    if (p == nullptr) {
        return nullptr;
    }

    p->_max = (size < MG_MAX_ALLOC_FROM_POOL) ? size : MG_MAX_ALLOC_FROM_POOL;
    p->_current = &p->_small;
    MG_LIST_INIT(&p->_large);
    MG_LIST_INIT(&p->_small);

    return p;
}

void mg_destory_pool(mg_pool* pool) {
    mg_list_t *h, *n;
    mg_large_block* l;
    mg_small_block* s;

    for (h = &pool->_large, n = h->_next; n != h; n = n->_next) {
        l = MG_STRUCT_POINT(mg_large_block, n, _l);
        mg_free(l->_head);
    }

    for (h = &pool->_small, n = h->_next; n != h; n = n->_next) {
        s = MG_STRUCT_POINT(mg_small_block, n, _l);
        mg_free(s->_head);
    }

    mg_free(pool);
}

static inline void* mg_palloc_large(mg_pool* pool, mg_size_t size) {
    void* p;
    mg_large_block* large;
    p = mg_malloc(size + sizeof(mg_large_block));
    if (p == nullptr) {
        return nullptr;
    }

    large = (mg_large_block*)p;
    large->_head = p;
    large->_buf = large + 1;
    MG_LIST_PUSH_BACK(&large->_l, &pool->_large);

    return large->_buf;
}

static inline void* mg_palloc_small(mg_pool* pool, mg_size_t size) {
    mg_list_t* p = pool->_current;
    mg_char_t* m;

    mg_small_block *s, *bl;

    while (p && p != &pool->_small) {
        s = MG_STRUCT_POINT(mg_small_block, p, _l);
        m = s->_last;
        if ((mg_size_t)(s->_end - m) >= size) {
            s->_last = m + size;
            return m;
        }
        p = p->_next;
    }

    m = (mg_char_t*)mg_malloc(MG_PAGESIZE);
    if (m == nullptr) {
        return nullptr;
    }

    bl = (mg_small_block*)m;
    bl->_head = m;
    bl->_end = m + MG_PAGESIZE - 1;
    bl->_failed = 0;

    m = (mg_char_t*)(bl + 1);
    bl->_last = m + size;

    for (p = pool->_current; p->_next != &pool->_small; p = p->_next) {
        s = MG_STRUCT_POINT(mg_small_block, p, _l);
        if (s->_failed++ > 4) {
            pool->_current = p->_next;
        }
    }

    MG_LIST_PUSH_BACK(&bl->_l, &pool->_small);
    return m;
}

void* mg_palloc(mg_pool* pool, mg_size_t size) {
    if (size <= pool->_max) {
        return mg_palloc_small(pool, size);
    }
    return mg_palloc_large(pool, size);
}

#endif
