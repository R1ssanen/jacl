#ifndef JAC_SYMBOL_H_
#define JAC_SYMBOL_H_

#include "darray.h"
#include "type.h"

struct jac_token;

typedef struct jac_symbol {
    const struct jac_token* identifier;
    jac_type                type;
} jac_symbol;

typedef struct jac_function {
    jac_type                ret_type;
    const struct jac_token* identifier;
    const darray_t char*    mangled;
    darray_t jac_symbol*    args;
} jac_function;

const char* jac_mangle_function_name(const struct jac_token* identifier, darray_t jac_symbol* args);

#endif
