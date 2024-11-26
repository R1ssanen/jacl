#ifndef _HASH_H_
#define _HASH_H_

#include <string.h>

#include "types.h"

static inline u64 FNV_1A(const char* str) {
    u64 hash = 0xcbf29ce484222325;

    for (u64 i = 0; i < strlen(str); ++i) {
        hash ^= str[i];
        hash *= 0x100000001b3;
    }

    return hash;
}

#endif
