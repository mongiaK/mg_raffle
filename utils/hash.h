/*================================================================
*  
*  文件名称：hash.h
*  创 建 者: mongia
*  创建日期：2020年07月02日
*  
================================================================*/

#pragma once

#include <stdint.h>
#include <string.h>

inline bool is_little() {
    union little {
        char c;
        int32_t i;
    };
    static little l = {1};
    return l.c == 1;
}


static uint32_t DecodeFixed32(const char* c) {
    if(is_little()) {
        uint32_t result;
        memcpy(&result, c, sizeof(result));
        return result;
    } else {
        return static_cast<uint32_t>(static_cast<uint8_t>(c[0])) |
            static_cast<uint32_t>(static_cast<uint8_t>(c[1]) << 8) |
            static_cast<uint32_t>(static_cast<uint8_t>(c[2]) << 16) |
            static_cast<uint32_t>(static_cast<uint8_t>(c[3]) << 24);
    }
}

static uint32_t murmur_hash(const char* data, size_t n, uint32_t seed) {
    // Similar to murmur hash
    const uint32_t m = 0xc6a4a793;
    const uint32_t r = 24;
    const char* limit = data + n;
    uint32_t h = seed ^ (n * m);

    // Pick up four bytes at a time
    while (data + 4 <= limit) {
        uint32_t w = DecodeFixed32(data);
        data += 4;
        h += w;
        h *= m;
        h ^= (h >> 16);
    }

    // Pick up remaining bytes
    switch (limit - data) {
        case 3:
            h += static_cast<unsigned char>(data[2]) << 16;
        case 2:
            h += static_cast<unsigned char>(data[1]) << 8;
        case 1:
            h += static_cast<unsigned char>(data[0]);
            h *= m;
            h ^= (h >> r);
            break;
    }
    return h;
}
 
