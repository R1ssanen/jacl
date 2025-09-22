#include "ir.h"

#include <stdarg.h>
#include <stdbool.h>

#include "assert.h"
#include "ast.h"
#include "darray.h"
#include "diagnostics.h"
#include "symbol.h"
#include "type.h"

typedef struct checker checker;

struct checker
{
    jac_ir_unit *unit;
    jac_ir_function *parent_fn;
    jac_ir_block *parent_block;
};

static bool check_expression(checker *checker, const jac_ast_expression *expression, jac_ir_block *block,
                             darray_t jac_ir_value **values)
{
    switch (expression->holds)
    {
    case JAC_AST_EXPRESSION_LITERAL: {
        jac_ir_value literal;
        literal.holds = JAC_IR_VALUE_LITERAL;
        literal.opt.literal.value = expression->opt.literal;
        darray_push(*values, literal);
    }
    break;

    default:
        JAC_ASSERT(false, "unreachable.");
    }

    return true;
}

static bool check_statement_return(checker *checker, const jac_ast_statement_return *ret, jac_ir_block *block)
{
    jac_ir_value *operands = darray_new(jac_ir_value);
    if (!check_expression(checker, ret->expression, block, &operands))
        return false;

    // jac_ir_symbol result = {
    //     .id    = ir->active_func->tmp_counter++,
    //     .label = NULL,
    //     .type  = (jac_ir_type){ .kind = JAC_IR_INT32, .ptr_indirection = 0 },
    // };
    jac_ir_instruction instruction = {.op = JAC_IR_RET, .operands = operands, .result = (jac_ir_symbol){0}};
    darray_push(block->instructions, instruction);
    return true;
}

static bool check_block(checker *checker, const jac_ast_block block)
{
    jac_ir_block new_block = {
        .id = checker->parent_fn->tmp_counter++,
        .instructions = darray_new(jac_ir_instruction),
    };

    darray_foreach(block, jac_ast_scope_statement, statement)
    {
        switch (statement->holds)
        {
        case JAC_AST_STATEMENT_RETURN:
            if (!check_statement_return(checker, statement->opt.ret, &new_block))
                return false;
            darray_push(checker->parent_fn->blocks, new_block);

            new_block = (jac_ir_block){
                .id = checker->parent_fn->tmp_counter++,
                .instructions = darray_new(jac_ir_instruction),
            };
            break;

        default:
            JAC_ASSERT(false, "unreachable.");
        }
    }

    darray_push(checker->parent_fn->blocks, new_block);
    return true;
}

static bool check_function_header(checker *checker, const jac_function *header, bool is_declaration)
{
    darray_foreach(checker->unit->functions, jac_ir_function, func_symbol)
    {
        if (strcmp(func_symbol->header.mangled, header->mangled) != 0)
            continue;

        jac_print_diagnostic(header->identifier, "redeclaration of function '%s'.", header->identifier->value);
        return false;
    }

    jac_ir_function new_func = {
        .header = *header,
        .blocks = is_declaration ? NULL : darray_new(jac_ir_block),
        .tmp_counter = 0,
    };

    darray_push(checker->unit->functions, new_func);
    checker->parent_fn = darray_last(checker->unit->functions);
    return true;
}

static bool check_function_definition(checker *checker, const jac_ast_function_definition *func_def)
{
    if (!check_function_header(checker, &func_def->header, false))
        return false;
    return check_block(checker, func_def->block);
}

bool jac_check_unit(const jac_ast_unit unit, jac_ir_unit *ir)
{
    *ir = (jac_ir_unit){
        .functions = darray_new(jac_ir_function),
        .globals = darray_new(jac_symbol),
        .str_literals = darray_new(const char *),
    };

    checker checker = {
        .unit = ir,
        .parent_fn = NULL,
        .parent_block = NULL,
    };

    bool success = true;

    darray_foreach(unit, jac_ast_unit_statement, statement)
    {
        switch (statement->holds)
        {
        case JAC_AST_EXTERN_BLOCK:
            darray_foreach(statement->opt.extern_block, jac_function,
                           header) if (!check_function_header(&checker, header, true)) success = false;
            break;
        case JAC_AST_FUNCTION_DEFINITION:
            if (!check_function_definition(&checker, statement->opt.func_def))
                success = false;
            break;

        default:
            JAC_ASSERT(false, "unreachable.");
        }
    }

    return success;
}

/*
 * DEBUGGING
 */

// static void indented(int indent, const char* msg) { printf("%*s%s\n",
// indent, " ", msg); }

static void indented_v(int indent, const char *fmt, ...)
{
    printf("%*s", indent, " ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    putchar('\n');
}

static void print_block(const jac_ir_block *block, int indent)
{
    printf(".L%lu0\n", block->id);

    darray_foreach(block->instructions, jac_ir_instruction, instruction)
    {
        switch (instruction->op)
        {
        case JAC_IR_RET:
            indented_v(indent, "ret $%s", instruction->operands[0].opt.literal.value->value);
            break;

        default:
            JAC_ASSERT(false, "unreachable.");
        }
    }

    printf(".L%lu1\n", block->id);
}

static void print_function(const jac_ir_function *function, int indent)
{
    bool is_declaration = !function->blocks;
    if (is_declaration)
        printf("extern ");

    // function->header.identifier->value
    printf("%s %s: ", function->header.ret_type.token->value, function->header.mangled);

    if (function->header.args)
    {
        jac_symbol *arg;
        for (size_t i = 0; i < darray_count(function->header.args) - 1; ++i)
        {
            for (size_t j = 0; j < arg->type.indirection; ++j)
                putchar('*');
            arg = function->header.args + i;
            printf("%s %%%s, ", arg->type.token->value, arg->identifier->value);
        }

        arg = darray_last(function->header.args);
        for (size_t i = 0; i < arg->type.indirection; ++i)
            putchar('*');
        printf("%s %%%s\n", arg->type.token->value, arg->identifier->value);
    }

    if (is_declaration)
        return;
    darray_foreach(function->blocks, jac_ir_block, block) print_block(block, indent);
}

void jac_print_ir(const jac_ir_unit *unit, int indent)
{
    darray_foreach(unit->functions, jac_ir_function, function) print_function(function, indent);
}
