#ifndef _JMEM_H_
#define _JMEM_H_

#include "types.h"

#define J_ARENA_GROWTH_FACTOR 1.5f

typedef struct {
    char* block;
    u64   head;
    u64   capacity;
} jArenaMemory;

void  jInitMemArena(u64 bytes, jArenaMemory* mem);
char* _jArenaAlloc(u64 bytes, jArenaMemory* mem);
void  jWipeArena(u64 pos, jArenaMemory* mem);

#define J_ALLOC(type, mem)      (type*)_jArenaAlloc(sizeof(type), mem)
#define J_ALLOC_N(type, n, mem) (type*)_jArenaAlloc(sizeof(type) * (n), mem)

#endif
