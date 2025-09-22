#ifndef JAC_ARENA_H_
#define JAC_ARENA_H_

#include "darray.h"

typedef struct jac_memory_arena jac_memory_arena;

struct jac_memory_arena
{
    darray_t char *memory;
};

static inline void *jac_arena_alloc_impl(jac_memory_arena *arena, unsigned count)
{
    arena->memory = (char *)darray_grow_impl((size_t *)arena->memory, count);
    return (void *)(darray_last(arena->memory) - count);
}

#define jac_arena_alloc(arena, type) jac_arena_alloc_impl(arena, sizeof(type))

static inline void jac_free_arena(jac_memory_arena *arena)
{
    darray_free(arena->memory);
}

#endif
