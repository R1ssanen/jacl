#include "ir.h"

#include <stdarg.h>
#include <stdbool.h>

#include "assert.h"
#include "ast.h"
#include "darray.h"
#include "diagnostics.h"

static bool check_expression(
    const jac_ast_expression* expression, jac_ir_unit* ir, jac_ir_block* block,
    darray_t jac_ir_value** values
) {

    switch (expression->holds) {
    case JAC_AST_EXPRESSION_LITERAL: {
        jac_ir_value literal;
        literal.holds                 = JAC_IR_VALUE_LITERAL;
        literal.opt.literal.holds     = JAC_IR_LITERAL_INT32;
        literal.opt.literal.opt.value = expression->opt.literal->value;
        darray_push(*values, literal);
    } break;

    default: JAC_ASSERT(false, "unreachable.");
    }

    return true;
}

static bool
check_statement_return(const jac_ast_statement_return* ret, jac_ir_unit* ir, jac_ir_block* block) {

    jac_ir_value* operands = darray_new(jac_ir_value);
    if (!check_expression(ret->expression, ir, block, &operands)) return false;

    // jac_ir_symbol result = {
    //     .id    = ir->active_func->tmp_counter++,
    //     .label = NULL,
    //     .type  = (jac_ir_type){ .kind = JAC_IR_INT32, .ptr_indirection = 0 },
    // };
    jac_ir_instruction instruction = { .op = JAC_IR_RET, .operands = operands, .result = { 0 } };
    darray_push(block->instructions, instruction);
    return true;
}

static bool check_block(const jac_ast_block* block, jac_ir_unit* ir) {

    jac_ir_block new_block = {
        .id           = ir->active_func->tmp_counter++,
        .instructions = darray_new(jac_ir_instruction),
    };

    darray_foreach(block->statements, jac_ast_scope_statement, statement) {
        switch (statement->holds) {
        case JAC_AST_STATEMENT_RETURN:
            if (!check_statement_return(statement->opt.ret, ir, &new_block)) return false;
            darray_push(ir->active_func->blocks, new_block);

            new_block = (jac_ir_block){
                .id           = ir->active_func->tmp_counter++,
                .instructions = darray_new(jac_ir_instruction),
            };
            break;

        default: JAC_ASSERT(false, "unreachable.");
        }
    }

    darray_push(ir->active_func->blocks, new_block);
    return true;
}

static bool
check_function_header(const jac_ast_function_header* header, bool is_declaration, jac_ir_unit* ir) {

    darray_foreach(ir->functions, jac_ir_function, func_symbol) {
        if (strcmp(func_symbol->label, header->identifier->value) != 0) continue;

        // darray_foreach(header->parameters, jac_ast_variable_declaration, arg){ func_symbol->}

        jac_print_diagnostic(
            header->identifier, "redeclaration of function '%s'.", header->identifier->value
        );
        return false;
    }

    jac_ir_function new_func = {
        .args        = darray_count(header->parameters) == 0 ? NULL : darray_new(jac_ir_symbol),
        .blocks      = is_declaration ? NULL : darray_new(jac_ir_block),
        .label       = header->identifier->value,
        .mangled     = NULL,
        .ret_type    = ast_to_ir_type(header->return_type),
        .tmp_counter = 0,
    };

    darray_foreach(header->parameters, jac_ast_variable_declaration, var_decl) {
        jac_ir_symbol func_arg = {
            .id    = 0, // ir->active_func->tmp_counter++,
            .label = var_decl->identifier->value,
            .type  = ast_to_ir_type(var_decl->type),
        };
        darray_push(new_func.args, func_arg);
    }

    darray_push(ir->functions, new_func);
    ir->active_func = darray_last(ir->functions);
    return true;
}

static bool
check_function_definition(const jac_ast_function_definition* func_def, jac_ir_unit* ir) {
    if (!check_function_header(func_def->header, false, ir)) return false;
    return check_block(func_def->block, ir);
}

bool jac_check_unit(const jac_ast_unit* unit, jac_ir_unit* ir) {

    *ir = (jac_ir_unit){
        .functions    = darray_new(jac_ir_function),
        .globals      = darray_new(jac_ir_symbol),
        .str_literals = darray_new(const char*),
    };
    bool success = true;

    darray_foreach(unit->statements, jac_ast_unit_statement, statement) {
        switch (statement->holds) {
        case JAC_AST_EXTERN_BLOCK:
            darray_foreach(
                statement->opt.extern_block->declarations, jac_ast_function_header, header
            ) if (!check_function_header(header, true, ir)) success = false;
            break;
        case JAC_AST_FUNCTION_DEFINITION:
            if (!check_function_definition(statement->opt.func_def, ir)) success = false;
            break;

        default: JAC_ASSERT(false, "unreachable.");
        }
    }

    return success;
}

/*
 * DEBUGGING
 */

// static void indented(int indent, const char* msg) { printf("%*s%s\n", indent, " ", msg); }

static void indented_v(int indent, const char* fmt, ...) {
    printf("%*s", indent, " ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    putchar('\n');
}

void print_block(const jac_ir_block* block, int indent) {
    printf(".L%lu0\n", block->id);

    darray_foreach(block->instructions, jac_ir_instruction, instruction) {
        switch (instruction->op) {
        case JAC_IR_RET:
            indented_v(indent, "ret $%s", instruction->operands[0].opt.literal.opt.value);
            break;

        default: JAC_ASSERT(false, "unreachable.");
        }
    }

    printf(".L%lu1\n", block->id);
}

void print_function(const jac_ir_function* function, int indent) {
    bool is_declaration = darray_count(function->blocks) == 0;
    if (is_declaration) printf("extern ");

    printf("%s %s: ", (function->ret_type.kind == JAC_IR_INT32) ? "i32" : "idk", function->label);

    if (function->args) {
        darray_foreach(function->args, jac_ir_symbol, arg) {
            printf("%s ", (arg->type.kind == JAC_IR_INT32) ? "i32" : "idk");
            putchar('%');
            if (arg->label) printf("%s, ", arg->label);
            else
                printf("t%lu, ", arg->id);
        }
    }
    putchar('\n');

    if (is_declaration) return;
    darray_foreach(function->blocks, jac_ir_block, block) print_block(block, indent);
}

void jac_print_ir(const jac_ir_unit* unit, int indent) {
    darray_foreach(unit->functions, jac_ir_function, function) print_function(function, indent);
}
