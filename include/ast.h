#ifndef JAC_AST_H_
#define JAC_AST_H_

#include <stdbool.h>

#include "darray.h"
#include "lex.h"

typedef enum jac_node_kind {
    JAC_NODE_FUNCTION_DEFINITION,
    JAC_NODE_FUNCTION_DECLARATION,
    JAC_NODE_STATEMENT_RETURN,
    JAC_NODE_STATEMENT_EXPR,
    JAC_NODE_STATEMENT_EXPR_CALL,
    JAC_NODE_EXPRESSION_LITERAL,
    JAC_NODE_EXPRESSION_IDENTIFIER,
    JAC_NODE_EXPRESSION_REF,
    JAC_NODE_VARIABLE_DECLARATION,
    JAC_NODE_BLOCK,
    JAC_NODE_TYPE,
    JAC_NODE_TYPE_POINTER,
    JAC_NODE_TYPE_NORMAL,
} jac_node_kind;

typedef struct jac_node_unit {
    darray_t struct jac_node_unit_statement* statements;
} jac_node_unit;

typedef struct jac_node_unit_statement {
    union {
        struct jac_node_function_definition*  func_def;
        struct jac_node_function_declaration* func_decl;
    } opt;

    jac_node_kind holds;
} jac_node_unit_statement;

typedef struct jac_node_function_header {
    darray_t struct jac_node_variable_declaration* parameters;
    struct jac_node_type*                          return_type;
    const jac_token*                               identifier;
} jac_node_function_header;

typedef struct jac_node_block {
    struct darray_t jac_node_scope_statement* statements;
} jac_node_block;

typedef struct jac_node_function_definition {
    jac_node_function_header* header;
    jac_node_block*           block;
} jac_node_function_definition;

typedef struct jac_node_function_declaration {
    jac_node_function_header* header;
} jac_node_function_declaration;

typedef struct jac_node_statement_expression_call {
    const jac_token*                     identifier;
    darray_t struct jac_node_expression* arguments;
} jac_node_statement_expression_call;

typedef struct jac_node_statement_expression {
    union {
        jac_node_statement_expression_call* call;
    } opt;

    jac_node_kind holds;
} jac_node_statement_expression;

typedef struct jac_node_expression {
    union {
        const jac_token*               literal;
        const jac_token*               identifier;
        const jac_token*               reference;
        jac_node_statement_expression* statement_expr;
    } opt;

    jac_node_kind holds;
} jac_node_expression;

typedef struct jac_node_statement_return {
    jac_node_expression* expression;
} jac_node_statement_return;

typedef struct jac_node_scope_statement {
    union {
        jac_node_statement_return*     return_;
        jac_node_block*                block;
        jac_node_statement_expression* expr_statement;
    } opt;

    jac_node_kind holds;
} jac_node_scope_statement;

typedef struct jac_node_type {
    union {
        struct jac_node_type* pointer;
        const jac_token*      normal;
    } opt;

    jac_node_kind holds;
} jac_node_type;

typedef struct jac_node_variable_declaration {
    jac_node_type*   type;
    const jac_token* identifier;
} jac_node_variable_declaration;

struct jac_memory_arena;
bool jac_parse_unit(
    struct jac_memory_arena* arena, darray_t jac_token* tokens, jac_node_unit* unit
);

void jac_free_unit(jac_node_unit* unit);

void jac_print_unit(const jac_node_unit* unit, int indent);

#endif
