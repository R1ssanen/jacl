#include "array.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "log.h"
#include "types.h"

static u64 _head_stride = JA_END_HEADER * sizeof(u64);

// create dynamic array with 1 or n spaces initialized
void* _jArrayCreate(u64 stride, u64 init_capacity) {
    u64  capacity = init_capacity ? init_capacity : 1;
    u64* header   = malloc(_head_stride + capacity * stride);
    if (!header) {
        J_FATAL("Could not allocate memory for dynamic array.\n %s\n", jGetErrnoString());
        abort();
    }

    header[JA_STRIDE]   = stride;
    header[JA_SIZE]     = 0;
    header[JA_CAPACITY] = capacity;

    return (void*)(header + JA_END_HEADER);
}

void _jArrayDestroy(void* arr) {
    u64* header = _jArrayGetHeader(arr);
    free(header);
}

void* _jArrayResize(void* arr) {
    u64* header       = _jArrayGetHeader(arr);
    u64  new_capacity = ceilf(header[JA_CAPACITY] * J_ARRAY_GROWTH_FACTOR);
    u64  total_bytes  = _head_stride + new_capacity * header[JA_STRIDE];

    header            = realloc(header, total_bytes);
    if (!header) {
        J_FATAL(
            "Could not reallocate %luB of array memory.\n %s\n", total_bytes, jGetErrnoString()
        );
        abort();
    }

    header[JA_CAPACITY] = new_capacity;
    return (void*)(header + JA_END_HEADER);
}

void* _jArrayPush(void* arr, const void* value) {
    u64* header = _jArrayGetHeader(arr);
    if (header[JA_CAPACITY] <= header[JA_SIZE]) {
        arr    = _jArrayResize(arr);
        header = _jArrayGetHeader(arr);
    }

    u64 stride = header[JA_STRIDE];
    u64 length = header[JA_SIZE]++;
    u8* addr   = (u8*)arr + length * stride;

    memcpy((void*)addr, value, stride);
    return arr;
}
