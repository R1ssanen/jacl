#ifndef JAC_IR_H_
#define JAC_IR_H_

#include <stdbool.h>

#include "darray.h"
#include "type.h"

typedef enum jac_ir_op {
    JAC_IR_RET,
    JAC_IR_CALL,
    JAC_IR_ADDR,
    JAC_IR_STORE,
    JAC_IR_LOAD,
} jac_ir_op;

typedef struct jac_ir_symbol {
    const char* label;
    jac_type    type;
    size_t      id;
} jac_ir_symbol;

typedef struct jac_ir_literal {
    union {
        size_t      str_index; // index to string table
        const char* value;
    } opt;

    enum {
        JAC_IR_LITERAL_STR,
        JAC_IR_LITERAL_INT64,
        JAC_IR_LITERAL_UINT32,
        JAC_IR_LITERAL_INT32,
        JAC_IR_LITERAL_UINT8,
    } holds;
} jac_ir_literal;

typedef struct jac_ir_value {
    union {
        jac_ir_symbol  variable;
        jac_ir_literal literal;
    } opt;

    enum {
        JAC_IR_VALUE_VARIABLE,
        JAC_IR_VALUE_LITERAL,
    } holds;
} jac_ir_value;

typedef struct jac_ir_instruction {
    jac_ir_symbol          result;
    darray_t jac_ir_value* operands;
    jac_ir_op              op;
} jac_ir_instruction;

typedef struct jac_ir_block {
    darray_t jac_ir_instruction* instructions;
    size_t                       id;
} jac_ir_block;

typedef struct jac_ir_function {
    jac_type                ret_type;
    const char *            label, *mangled;
    darray_t jac_ir_symbol* args;
    darray_t jac_ir_block*  blocks;
    size_t                  tmp_counter;
} jac_ir_function;

typedef struct jac_ir_unit {
    darray_t jac_ir_function* functions;
    darray_t jac_ir_symbol*   globals;
    const darray_t char**     str_literals;
} jac_ir_unit;

struct jac_ast_unit;

bool jac_check_unit(const struct jac_ast_unit* unit, jac_ir_unit* ir);

void jac_print_ir(const jac_ir_unit* unit, int indent);

#endif
