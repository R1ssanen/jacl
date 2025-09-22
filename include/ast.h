#ifndef JAC_AST_H_
#define JAC_AST_H_

#include <stdbool.h>

#include "darray.h"
#include "lex.h"
#include "symbol.h"

typedef struct jac_ast_unit_statement jac_ast_unit_statement;
typedef struct jac_ast_function_definition jac_ast_function_definition;
typedef struct jac_ast_function_call jac_ast_function_call;
typedef struct jac_ast_variable_definition jac_ast_variable_definition;
typedef struct jac_ast_assignment jac_ast_assignment;
typedef struct jac_ast_expression_statement jac_ast_expression_statement;
typedef struct jac_ast_expression jac_ast_expression;
typedef struct jac_ast_statement_return jac_ast_statement_return;
typedef struct jac_ast_scope_statement jac_ast_scope_statement;

typedef darray_t jac_function *jac_ast_extern_block;
typedef darray_t jac_ast_unit_statement *jac_ast_unit;
typedef darray_t jac_ast_scope_statement *jac_ast_block;

enum jac_ast_kind
{
    JAC_AST_FUNCTION_DEFINITION,
    JAC_AST_EXTERN_BLOCK,
    JAC_AST_FUNCTION_HEADER,
    JAC_AST_STATEMENT_RETURN,
    JAC_AST_EXPRESSION_STATEMENT,
    JAC_AST_FUNCTION_CALL,
    JAC_AST_EXPRESSION_LITERAL,
    JAC_AST_EXPRESSION_IDENTIFIER,
    JAC_AST_EXPRESSION_ADDROF,
    JAC_AST_VARIABLE_DECLARATION,
    JAC_AST_VARIABLE_DEFINITION,
    JAC_AST_BLOCK,
    JAC_AST_TYPE,
    JAC_AST_TYPE_POINTER,
    JAC_AST_TYPE_NORMAL,
    JAC_AST_ASSIGNMENT,
};

struct jac_ast_unit_statement
{
    union {
        jac_ast_function_definition *func_def;
        jac_ast_extern_block extern_block;
    } opt;

    enum jac_ast_kind holds;
};

struct jac_ast_function_definition
{
    jac_function header;
    jac_ast_block block;
};

struct jac_ast_function_call
{
    const jac_token *identifier;
    darray_t jac_ast_expression *arguments;
};

struct jac_ast_variable_definition
{
    jac_symbol declaration;
    jac_ast_expression *expression;
};

struct jac_ast_assignment
{
    const jac_token *identifier;
    jac_ast_expression *expression;
};

struct jac_ast_expression_statement
{
    union {
        jac_ast_function_call *call;
        jac_ast_variable_definition *var_def;
        jac_ast_assignment *assignment;
    } opt;

    enum jac_ast_kind holds;
};

struct jac_ast_expression
{
    union {
        const jac_token *literal;
        const jac_token *identifier;
        const jac_token *addrof;
        jac_ast_expression_statement *expr_statement;
    } opt;

    enum jac_ast_kind holds;
};

struct jac_ast_statement_return
{
    jac_ast_expression *expression;
};

struct jac_ast_scope_statement
{
    union {
        jac_ast_block block;
        jac_ast_statement_return *ret;
        jac_ast_expression_statement *expr_statement;
    } opt;

    enum jac_ast_kind holds;
};

struct jac_memory_arena;

bool jac_parse_unit(struct jac_memory_arena *arena, darray_t jac_token *tokens, jac_ast_unit *unit);

void jac_free_unit(jac_ast_unit unit);

void jac_print_unit(const jac_ast_unit unit, int indent);

#endif
