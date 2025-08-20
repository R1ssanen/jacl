#ifndef JAC_STACK_H_
#define JAC_STACK_H_

#include <stddef.h>

#include "darray.h"

typedef struct jac_variable {
    const struct jac_token* identifier;
    size_t                  offset;
    long                    value;
} jac_variable;

typedef struct jac_stack {
    size_t                 sp;
    darray_t jac_variable* variables;
    darray_t size_t*       frames;
} jac_stack;

static inline size_t jac_insert_stack_frame(jac_stack* stack) {
    size_t base = stack->sp;
    darray_push(stack->frames, base);
    return base;
}

static inline void jac_insert_variable(jac_stack* stack, const jac_token* identifier, long value) {
    stack->sp += 8;
    jac_variable variable = { .identifier = identifier,
                              .offset     = stack->sp - (*darray_last(stack->frames)),
                              .value      = value };

    darray_push(stack->variables, variable);
}

#endif
