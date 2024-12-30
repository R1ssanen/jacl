#include "array.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "types.h"

static u64 _head_stride = JA_END_HEADER * sizeof(u64);

u64*       _jArrayGetHeader(void* arr) { return (u64*)arr - JA_END_HEADER; }

// create dynamic array with 1 space
void* _jArrayCreate(u64 stride) {
    u64* header = malloc(_head_stride + stride);
    if (!header) {
        fprintf(
            stderr, "jacl: critical error, could not allocate memory for array header.\n %s\n",
            jGetErrnoString()
        );
        abort();
    }

    header[JA_STRIDE]   = stride;
    header[JA_SIZE]     = 0;
    header[JA_CAPACITY] = 1;

    _J_ARRAY_DEBUG_MSG("array head stride of %luB.\n", _head_stride);
    _J_ARRAY_DEBUG_MSG("array created with stride of %luB.\n", stride);
    return (void*)(header + JA_END_HEADER);
}

void _jArrayDestroy(void* arr) {
    u64* header = _jArrayGetHeader(arr);
    _J_ARRAY_DEBUG_MSG(
        "array destroyed with %lu elements out of %lu.\n", header[JA_SIZE], header[JA_CAPACITY]
    );
    free(header);
}

void _jArrayResize(void* arr) {
    u64* header       = _jArrayGetHeader(arr);
    u64  new_capacity = ceilf(header[JA_CAPACITY] * J_ARRAY_GROWTH_FACTOR);
    u64  total_bytes  = _head_stride + new_capacity * header[JA_STRIDE];

    _J_ARRAY_DEBUG_MSG(
        "resizing array from capacity of %lu (%luB) to %lu (%luB).\n", header[JA_CAPACITY],
        header[JA_CAPACITY] * header[JA_STRIDE], new_capacity, new_capacity * header[JA_STRIDE]
    );

    header = realloc(header, total_bytes);
    if (!header) {
        fprintf(
            stderr, "jacl: critical error, could not reallocate %luB of array memory.\n %s\n",
            total_bytes, jGetErrnoString()
        );
        abort();
    }

    header[JA_CAPACITY] = new_capacity;
}
