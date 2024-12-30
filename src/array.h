#ifndef _JARRAY_H_
#define _JARRAY_H_

#include "types.h"

#define J_ARRAY_GROWTH_FACTOR 1.5f

#ifdef J_DEBUG_ARRAYS
#    define _J_ARRAY_DEBUG_MSG(fmt, ...)                                                           \
        fputs("jarrd: ", stderr);                                                                  \
        fprintf(stderr, fmt, ##__VA_ARGS__);
#else
#    define _J_ARRAY_DEBUG_MSG(fmt, ...)
#endif

enum j_array_header_t {
    JA_STRIDE = 0,
    JA_SIZE,
    JA_CAPACITY,
    JA_END_HEADER,
};

u64*  _jArrayGetHeader(void* arr);
void* _jArrayCreate(u64 stride);
void  _jArrayDestroy(void* arr);
void  _jArrayResize(void* arr);

#define J_ARRAY_CREATE(type) (type*)_jArrayCreate(sizeof(type))
#define J_ARRAY_DESTROY(arr) _jArrayDestroy(arr)
#define J_ARRAY_SIZE(arr)    (_jArrayGetHeader(arr)[JA_SIZE])

#define J_ARRAY_PUSH(arr, value)                                                                   \
    do {                                                                                           \
        u64* header = _jArrayGetHeader(arr);                                                       \
        _J_ARRAY_DEBUG_MSG("pushing to array index %lu.\n", header[JA_SIZE]);                      \
        if (header[JA_CAPACITY] <= header[JA_SIZE]) _jArrayResize(arr);                            \
        arr[header[JA_SIZE]++] = value;                                                            \
    } while (false)

#define J_ARRAY_REMOVE(arr, index)                                                                 \
    do {                                                                                           \
        _J_ARRAY_DEBUG_MSG("removing from array index %lu.\n", index);                             \
        u64* header = _jArrayGetHeader(arr);                                                       \
        u64  last   = header[JA_SIZE] - 1;                                                         \
        if (last < 0 || index < 0) {                                                               \
            _J_ARRAY_DEBUG_MSG("cannot remove from empty array or negative index.\n");             \
            break;                                                                                 \
        }                                                                                          \
        header[JA_SIZE]--;                                                                         \
        for (u64 i = index; i < last; ++i) arr[i] = arr[i + 1];                                    \
    } while (false)

#define J_ARRAY_POP(arr)                                                                           \
    do {                                                                                           \
        u64* header = _jArrayGetHeader(arr);                                                       \
        _J_ARRAY_DEBUG_MSG("popping from array, last element at %lu.\n", header[JA_SIZE] - 1);     \
        if (header[JA_SIZE] < 1) {                                                                 \
            _J_ARRAY_DEBUG_MSG("cannot pop from empty array.\n");                                  \
            break;                                                                                 \
        }                                                                                          \
        header[JA_SIZE]--;                                                                         \
    } while (false)

#endif
