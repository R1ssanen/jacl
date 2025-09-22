#ifndef JAC_IR_H_
#define JAC_IR_H_

#include <stdbool.h>

#include "ast.h"
#include "darray.h"
#include "symbol.h"
#include "type.h"

typedef struct jac_ir_symbol jac_ir_symbol;
typedef struct jac_ir_literal jac_ir_literal;
typedef struct jac_ir_value jac_ir_value;
typedef struct jac_ir_instruction jac_ir_instruction;
typedef struct jac_ir_block jac_ir_block;
typedef struct jac_ir_function jac_ir_function;
typedef struct jac_ir_unit jac_ir_unit;

enum jac_ir_op
{
    JAC_IR_RET,
    JAC_IR_CALL,
    JAC_IR_ADDR,
    JAC_IR_STORE,
    JAC_IR_LOAD,
};

struct jac_ir_symbol
{
    jac_symbol sym;
    size_t id;
};

struct jac_ir_literal
{
    size_t str_index;
    const struct jac_token *value;
};

struct jac_ir_value
{
    union {
        jac_ir_symbol variable;
        jac_ir_literal literal;
    } opt;

    enum
    {
        JAC_IR_VALUE_VARIABLE,
        JAC_IR_VALUE_LITERAL,
    } holds;
};

struct jac_ir_instruction
{
    jac_ir_symbol result;
    darray_t jac_ir_value *operands;
    enum jac_ir_op op;
};

struct jac_ir_block
{
    darray_t jac_ir_instruction *instructions;
    size_t id;
};

struct jac_ir_function
{
    jac_function header;
    darray_t jac_ir_block *blocks;
    size_t tmp_counter;
};

struct jac_ir_unit
{
    darray_t jac_ir_function *functions;
    darray_t jac_symbol *globals;
    const darray_t char **str_literals;
};

bool jac_check_unit(const jac_ast_unit unit, jac_ir_unit *ir);

void jac_print_ir(const jac_ir_unit *unit, int indent);

#endif
