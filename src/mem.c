#include "mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "types.h"

void jInitMemArena(u64 bytes, jArenaMemory* mem) {
    mem->block = calloc(bytes, 1);
    if (!mem->block) {
        fprintf(
            stderr, "jacl: critical error, could not allocate %luB of arena memory.\n %s\n",
            mem->capacity, jGetErrnoString()
        );
        abort();
    }

    mem->capacity = bytes;
    mem->head     = 0;
}

char* _jArenaAlloc(u64 bytes, jArenaMemory* mem) {
    u64 old_head = mem->head;
    mem->head += bytes;

    if (mem->head > mem->capacity) {
        mem->capacity *= J_ARENA_GROWTH_FACTOR;
        mem->block = realloc(mem->block, mem->capacity);
        if (!mem->block) {
            fprintf(
                stderr,
                "jacl: critical error, could not resize arena memory to size of %luB.\n %s\n",
                mem->capacity, jGetErrnoString()
            );
            abort();
        }
    }

    return mem->block + old_head;
}

void jWipeArena(u64 pos, jArenaMemory* mem) {
    if (pos < 0) { return; }
    mem->head -= mem->head - pos;
}
