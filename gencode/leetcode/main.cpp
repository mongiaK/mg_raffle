/*================================================================
*
*  文件名称：main.cpp
*  创 建 者: mongia
*  创建日期：2023年07月05日
*
================================================================*/

#include <bits/stdc++.h>
#include <cstddef>
#include <cstdint>
#include <getopt.h>
#include <iostream>
#include <string>

using namespace std;

char *cmd_long_options_to_short_options(const struct option options[]) {
    char short_options[UCHAR_MAX * 3 + 1];
    char *p = short_options;

    for (; options->name; options++) {
        const struct option *o = options;
        if (o->flag == NULL && o->val > 0 && o->val <= UCHAR_MAX) {
            *p++ = o->val;
            if (o->has_arg == required_argument) {
                *p++ = ':';
            } else if (o->has_arg == optional_argument) {
                *p++ = ':';
                *p++ = ':';
            }
        }
    }
    *p++ = '\0';

    return strdup(short_options);
}

void parse_options(int argc, char *argv[]) {
    static const struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {NULL, 0, NULL, 0},
    };

    char *short_options = cmd_long_options_to_short_options(long_options);
    for (;;) {
        int c = getopt_long(argc, argv, short_options, long_options, NULL);
        if (-1 == c) {
            break;
        }

        switch (c) {
        case 'h':
            break;
        case 'v':
            break;
        default:
            abort();
        }
    }

    free(short_options);
}

string longestPalindrome(string s) {
    int length = s.length();
    int max_length = 0;
    int max_index = 0;
    int len = 0;
    for (int i = 0; i < length; i++) {
        int istart = i;
        for (int j = length - 1; j >= i; j--) {
            int jstart = j;
            while (istart <= jstart) {
                if (s[istart] == s[jstart]) {
                    len += (istart == jstart) ? 1 : 2;
                    istart++;
                    jstart--;
                } else {
                    istart = i;
                    break;
                }
            }
            if (len > max_length) {
                max_length = len;
                max_index = i;
                break;
            }
            len = 0;
        }
    }

    return s.substr(max_index, max_length);
}

int pivotIndex(vector<int> &nums) {
    int left = 0;
    int right = 0;
    for (auto &it : nums) {
        right += it;
    }
    for (int i = 0; i < nums.size(); i++) {
        if (i > 0) {
            left += nums[i - 1];
        }
        right -= nums[i];
        if (left == right) {
            return i;
        }
    }
    return -1;
}

union testUnion {
    struct {
        uint32_t key;
        uint32_t value;
        int64_t type;
        int64_t flags;
    };

    struct {
        int32_t a;
        int32_t b;
        int32_t c;
        int64_t d;
        int64_t e;
        int64_t f;
    } test;
} __attribute__((aligned(8)));

#define offsetofend(type, field)                                               \
    (offsetof(type, field) + sizeof(((type *)0)->field))

uint64_t ptr_to_u64(void *ptr) { return uint64_t(ptr); }

#define __concat(a, b) a##b
#define __apply(fn, n) __concat(fn, n)
#define __nth(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, N, ...) N

#define __argn(...) __nth(_, ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define __empty(...) __nth(_, ##__VA_ARGS__, N, N, N, N, N, N, N, N, N, N, 0)


int main(int argc, char *argv[]) {
    /* parse_options(argc, argv); */

    vector<int> nums = {-1, -1, -1, 1, 1, 1};
    union testUnion un;

    std::cout << sizeof(union testUnion) << std::endl;
    std::cout << offsetofend(union testUnion, flags) << std::endl;

    un.key = 32;
    un.test.e = 43;

    std::cout << ptr_to_u64(&un) << std::endl;

    std::cout << pivotIndex(nums) << std::end;
    std::cout << longestPalindrome("aacabdkacaa") << std::endl;
    return 0;
}
