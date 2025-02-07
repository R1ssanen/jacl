#ifndef _JARRAY_H_
#define _JARRAY_H_

#include "types.h"

#define J_ARRAY_GROWTH_FACTOR 1.5f

enum {
    JA_STRIDE = 0,
    JA_SIZE,
    JA_CAPACITY,
    JA_END_HEADER,
};

static inline u64* _jArrayGetHeader(void* arr) { return (u64*)arr - JA_END_HEADER; }

void*              _jArrayCreate(u64 stride, u64 init_capacity);
void*              _jArrayResize(void* arr);
void               _jArrayDestroy(void* arr);
void*              _jArrayPush(void* arr, const void* value);

#define J_ARRAY_CREATE(type)      _jArrayCreate(sizeof(type), 0)
#define J_ARRAY_INIT(type, elems) _jArrayCreate(sizeof(type), (elems))
#define J_ARRAY_DESTROY(arr)      _jArrayDestroy(arr)
#define J_ARRAY_SIZE(arr)         (_jArrayGetHeader(arr)[JA_SIZE])

#define J_ARRAY_PUSH(arr, value)                                                                   \
    do {                                                                                           \
        __auto_type temp = value;                                                                  \
        arr              = _jArrayPush(arr, &temp);                                                \
    } while (false)

#endif
