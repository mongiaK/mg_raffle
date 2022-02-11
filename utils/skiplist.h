/*================================================================
*
*  文件名称：skiplist.h
*  创 建 者: mongia
*  创建日期：2022年01月25日
*  邮    箱：mr_pengmj@outlook.com
*
================================================================*/

#pragma once

#include <stdint.h>

#include <vector>

#ifdef MG_DEBUG
#include "log.h"
#endif

namespace mg {

template <class T>
class SkipList {
   public:
    SkipList(uint32_t level = 64) : _sk_level(level) {
        _head.resize(_sk_level);
    }
    ~SkipList() {}

    void insert(T& val) {}

    void del(T& val) {}

    void search(T& val) {}

   private:
    uint32_t _sk_level;
    std::vector<void*> _head;
};

}  // namespace mg
