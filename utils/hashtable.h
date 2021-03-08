#pragma once

#include <iostream>
#include <vector>
#include <atomic>

#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <stddef.h>

class Slice {
    private:
        const char* _data;
        uint32_t    _size;
    public:
        Slice(const char* data, uint32_t size) : _data(data), _size(size) {}
        const char* data() const { return _data; }
        const uint32_t size() const { return _size; }

        std::string to_string() { return std::string(_data, _size); } 
};

inline bool operator==(const Slice& left, const Slice& right) {
    return (left.size() == right.size()) &&
        (memcmp(left.data(), right.data(), left.size()) == 0);
}

struct List {
    List *prev, *next;
    List() {
        prev = next = nullptr;
    }
};

struct Node {
    uint32_t    _hash;
    size_t      _key_length;
    List        _lhash;
    char        _key_data[1];

    Slice key() const {
        return Slice(_key_data, _key_length);
    }
};


#define GET_ADDR_BY_MEMBER(l, type, member)  (type*)(reinterpret_cast<uintptr_t>(&l) - offsetof(type, member));

template <class T = Node>
class HashTable {
    private:
        uint32_t _length;
        uint32_t _elem;
        List** _list;
    public:
        HashTable() : _length(0), _elem(0), _list(nullptr) { resize(); }
    private:
        List** find_node(const Slice& key, uint32_t hash) {
            uint32_t index = hash & (_length - 1);
            List** ptr = &_list[index];
            while((*ptr) != nullptr) {
                T* t = GET_ADDR_BY_MEMBER(**ptr, T, _lhash);
                if(t->hash == hash && t->key() == key) {
                    return ptr;
                }
                ptr = &((*ptr)->next);
            }
            
            return ptr; 
        }
    public:
        List* lookup(const Slice& key, uint32_t hash) {
            return *find_node(key, hash);
        }

        List* remove_node(const Slice& key, uint32_t hash) {
            List** l = find_node(key, hash);
            if((*l) != nullptr) {
                (*l)->prev->next = (*l)->next;
                if((*l)->next != nullptr)
                    (*l)->next->prev = (*l)->prev;
                _elem--;
            }
            return *l;
        }

        List* insert_node(List* l) {
            T* t = GET_ADDR_BY_MEMBER(*l, T, _lhash);
            List** ptr = *find_node(t->key(), t->hash);  // 返回的是next的地址
            List* old_l = *ptr;  // 取出next的值
            *ptr = l;     // 将next的值换成新的插入的l的值
            l->prev = GET_ADDR_BY_MEMBER(*ptr, List, next);   // 根据next的值算出当前List的地址，当前l的前移值的地址指向该值  

            if(old_l == nullptr) {
                _elem++;
                if(_elem > _length) {
                    resize();
                }
            }
            
            return old_l;
        }

    private:
        void resize() {
            uint32_t new_length = 4;
            while(new_length < _elem) {
                new_length <<= 2;
            }
            List** new_list = new List*[new_length];
            memset(new_list, 0, new_length);
            uint32_t count = 0;
            for(uint32_t i = 0; i < _length; ++i) {
                List* l = _list[i];
                while(l != nullptr) {
                    List* next = l->next;
                    T* t = GET_ADDR_BY_MEMBER(*l, T, _lhash);
                    List** new_head = &new_list[t->hash & (new_length - 1)];
                    l->next = *new_head;
                    *new_head = l;
                    l = next;

                    count++;
                }
            }
            assert(count == _elem);
            delete [] _list;
            _list = new_list;
            _length = new_length;
        }
};
