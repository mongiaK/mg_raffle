/*================================================================
*
*  文件名称：algorithm.h
*  创 建 者: mongia
*  创建日期：2020年07月01日
*
================================================================*/

#pragma once

#include <cstdlib>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>

template <class T>
class Sequence {
   public:
    typedef int (*compare)(const T& tl, const T& tr);
    explicit Sequence(compare comp) : _comp(comp) {}

   private:
    compare _comp;

   public:
    void bubble(T* array, int length) {
        if (length < 2) {
            return;
        }
        int i, j, comp_ret;
        T tmp;
        for (i = length - 1; i < length; i--) {
            for (j = 0; j < i; j++) {
                comp_ret = _comp(array[i], array[j]);
                if (comp_ret > 0) {
                    tmp = array[i];
                    array[i] = array[j];
                    array[j] = tmp;
                }
            }
        }
    }

    void fast_sort(T* array, int low, int high) {
        if (low >= high) {
            return;
        }
        int i = low;
        int j = high;
        T key = array[low];
        while (i < j) {
            while (_comp(array[j], key) < 0) {
                array[i] = array[j];
                j--;
            }

            while (_comp(array[i], key) > 0) {
                array[j] = array[i];
                i++;
            }
        }
        array[i] = key;
        fast_sort(array, low, j - 1);
        fast_sort(array, j + 1, high);
    }

    void adjust_heap(T* array, int start, int end) {
        int dad = start;
        int son = dad * 2 + 1;
        // 从start开始调整堆，
        while (son <= end) {
            if (son + 1 <= end && _comp(array[son], array[son + 1]) < 0) {
                son += 1;
            }
            if (_comp(array[dad], array[son]) >
                0) {  // 因为堆是从下而上构造的，所以出现这种条件，下面的就不需要再遍历了。
                return;
            } else {
                T tmp = array[dad];
                array[dad] = array[son];
                array[son] = tmp;
                dad = son;
                son = dad * 2 + 1;
            }
        }
    }

    void heap_sort(T* array, int length) {
        int i;
        // 构造堆
        for (i = length / 2 - 1; i >= 0; i--) {
            // 从右至左，从下而上构造堆
            adjust_heap(array, i, length);
        }

        for (i = length - 1; i > 0; i--) {
            T tmp = array[0];
            array[0] = array[i];
            array[i] = tmp;
            adjust_heap(array, 0, i - 1);
        }
    }
};

template <typename T>
class Solution {
    private:
        std::mutex _mutex;
        std::condition_variable _cond;
        std::queue<T> _data;
    public:
        T&& get() {
            std::unique_lock<std::mutex> _(_mutex);
            if(_data.empty()) {
                _cond.wait(_);
            }
            T t = _data.front();
            _data.pop_front();

            return std::move(t);
        }

        void put(T& t) {
            std::unique_lock<std::mutex> _(_mutex);
            _data.push_back(t);
            _cond.notify_one();
        }
};

class Queens {
    private:
        int vec[9];
        int total;
    public:
        Queens() {
            total = 0;
            for(int i = 0; i < 9; i++)
                vec[i] = 0;
        }

        void print_queen() {
            for(int i = 1; i < 9; i++) {
                std::cout << " " << i << ", " << vec[i];
            }
            std::cout << std::endl;
        }

        bool check(int row, int col) {
            for(int i = 0; i < row; i++) {
                if(col == vec[i])
                    return false;
                if(abs(row - i) == abs(vec[row] - vec[i]))
                    return false;
            }
            return true;
        }

        void find_queue(int row) {
            if(row == 8) {
                total++;
                print_queen();
                return;
            }
            for(int i = 1; i < 9; i++) {
                vec[row] = i;
                if(check(row, i)){
                    row++;
                    find_queue(row);
                    row--;
                }
            }
        }
};
