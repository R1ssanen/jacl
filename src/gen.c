#include "gen.h"

#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "assert.h"
#include "ast.h"
#include "darray.h"
#include "symbol.h"

/*
 * GENERATOR
 */

typedef enum section {
    SECTION_DATA,
    SECTION_BSS,
    SECTION_RODATA,
    SECTION_TEXT,
    END_SECTION,
} section;

static const char* section_names[]      = { ".data", ".bss", ".rodata", ".text" };
static const char* argument_registers[] = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };

typedef struct generator {
    darray_t char*         sections[END_SECTION];
    const darray_t char**  str_literals;
    darray_t jac_function* functions;
} generator;

static char* concat(darray_t char* array, const char* str, size_t length) {
    size_t old_count = darray_count(array);
    array            = darray_grow_impl((size_t*)array, length);
    memcpy((void*)(array + old_count), str, length * sizeof(char));
    return array;
}

static void emit(generator*, section, const char*, ...);

static void
emit_impl(generator* gen, section section_, size_t indent, const char* fmt, va_list args) {
    char buffer[256];
    memset((void*)buffer, ' ', indent);
    size_t written = vsprintf(buffer + indent, fmt, args);

    if (!gen->sections[section_]) {
        gen->sections[section_] = darray_new(char);
        emit(gen, section_, "section %s\n", section_names[section_]);
    }
    gen->sections[section_] = concat(gen->sections[section_], buffer, written + indent);
}

static void emit(generator* gen, section section_, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    emit_impl(gen, section_, 8, fmt, args);
    va_end(args);
}

static void emit_noindent(generator* gen, section section_, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    emit_impl(gen, section_, 0, fmt, args);
    va_end(args);
}

static void init_generator(generator* gen) {
    for (size_t i = 0; i < END_SECTION; ++i) gen->sections[i] = NULL;
    gen->str_literals = darray_new(const char*);
    gen->functions    = darray_new(jac_function);
}

static void free_generator(generator* gen) {
    for (size_t i = 0; i < END_SECTION; ++i) {
        if (!gen->sections[i]) continue;
        darray_free(gen->sections[i]);
    }
    darray_free(gen->str_literals);
    darray_free(gen->functions);
}

static void push(generator* gen, const char* value) { emit(gen, SECTION_TEXT, "push %s\n", value); }

static void pop(generator* gen, const char* register_) {
    emit(gen, SECTION_TEXT, "pop %s\n", register_);
}

/*
 * CODEGEN
 */

static void generate_literal(generator* gen, const jac_token* literal) {
    switch (literal->kind) {
    case JAC_TOKEN_INT_LITERAL: push(gen, literal->value); break;
    case JAC_TOKEN_STR_LITERAL: {
        size_t str_count = darray_count(gen->str_literals);
        for (size_t i = 0; i < str_count; ++i) {
            if (strcmp(gen->str_literals[i], literal->value) == 0) {
                emit(gen, SECTION_TEXT, "push S%lu\n", i);
                return;
            }
        }

        emit_noindent(gen, SECTION_RODATA, "S%lu: db ", str_count);
        darray_push(gen->str_literals, literal->value);

        size_t len = darray_count(literal->value);
        for (size_t i = 0; i < len - 1; ++i)
            emit_noindent(gen, SECTION_RODATA, "0x%x, ", (int)literal->value[i]);

        emit_noindent(gen, SECTION_RODATA, "0x%x\n", (int)literal->value[len - 1]);
        emit(gen, SECTION_TEXT, "push S%lu\n", str_count);
    } break;

    default: JAC_ASSERT(false, "unreachable.");
    }
}

static void
generate_expression(generator*, const jac_ast_expression*, jac_ast_variable_definition**);

static void generate_statement_expression_call(
    generator* gen, const jac_ast_function_call* call, jac_ast_variable_definition** variables
) {

    for (size_t i = 0; i < darray_count(call->arguments); ++i) {
        generate_expression(gen, call->arguments + i, variables);
        pop(gen, argument_registers[i]);
    }

    // set %rax to 0 to signal no vector registers we're used
    emit(gen, SECTION_TEXT, "xor rax, rax\n");

    emit(gen, SECTION_TEXT, "call %s\n", call->identifier->value);
    push(gen, "rax");
}

static void generate_variable_definition(
    generator* gen, const jac_ast_variable_definition* var_def,
    jac_ast_variable_definition** variables
) {
    generate_expression(gen, var_def->expression, variables);
    emit(gen, SECTION_TEXT, "pop rax\n");
    emit(gen, SECTION_TEXT, "mov [rbp - %lu], rax\n", darray_count(*variables) * 8 + 8);
    darray_push(*variables, *var_def);
    return;
}

static void generate_assignment(
    generator* gen, const jac_ast_assignment* assignment, jac_ast_variable_definition** variables
) {
    darray_foreach(*variables, jac_ast_variable_definition, variable) {
        if (strcmp(variable->var_decl->identifier->value, assignment->identifier->value) == 0) {
            variable->expression = assignment->expression;
            generate_expression(gen, assignment->expression, variables);
            return;
        }
    }

    JAC_ASSERT(false, "undefined variable.");
}

static void generate_statement_expression(
    generator* gen, const jac_ast_expression_statement* expr_statement,
    jac_ast_variable_definition** variables
) {

    switch (expr_statement->holds) {
    case JAC_AST_FUNCTION_CALL:
        generate_statement_expression_call(gen, expr_statement->opt.call, variables);
        break;
    case JAC_AST_VARIABLE_DEFINITION:
        generate_variable_definition(gen, expr_statement->opt.var_def, variables);
        break;
    case JAC_AST_ASSIGNMENT:
        generate_assignment(gen, expr_statement->opt.assignment, variables);
        break;

    default: JAC_ASSERT(false, "unreachable.");
    }
}

static void generate_identifier(
    generator* gen, const jac_token* identifier, jac_ast_variable_definition** variables
) {
    darray_foreach(*variables, jac_ast_variable_definition, variable) {
        if (strcmp(variable->var_decl->identifier->value, identifier->value) == 0) {
            generate_expression(gen, variable->expression, variables);
            return;
        }
    }

    JAC_ASSERT(false, "undefined variable.");
}

static void generate_reference(
    generator* gen, const jac_token* addrof, jac_ast_variable_definition** variables_p
) {
    jac_ast_variable_definition* variables = *variables_p;

    for (size_t i = 0; i < darray_count(variables); ++i) {
        if (strcmp(variables[i].var_decl->identifier->value, addrof->value) == 0) {
            emit(gen, SECTION_TEXT, "lea rax, [rbp - %lu]\n", i * 8 + 8);
            emit(gen, SECTION_TEXT, "push rax\n");
            return;
        }
    }

    JAC_ASSERT(false, "undefined variable.");
}

static void generate_expression(
    generator* gen, const jac_ast_expression* expression, jac_ast_variable_definition** variables
) {

    switch (expression->holds) {
    case JAC_AST_EXPRESSION_LITERAL: generate_literal(gen, expression->opt.literal); break;
    case JAC_AST_EXPRESSION_STATEMENT:
        generate_statement_expression(gen, expression->opt.expr_statement, variables);
        break;
    case JAC_AST_EXPRESSION_IDENTIFIER:
        generate_identifier(gen, expression->opt.identifier, variables);
        break;
    case JAC_AST_EXPRESSION_ADDROF:
        generate_reference(gen, expression->opt.addrof, variables);
        break;

    default: JAC_ASSERT(false, "unreachable.");
    }
}

static void generate_statement_return(
    generator* gen, const jac_ast_statement_return* ret, jac_ast_variable_definition** variables
) {
    generate_expression(gen, ret->expression, variables);
    pop(gen, "rax");
    emit(gen, SECTION_TEXT, "jmp .ret\n");
}

static void generate_block(
    generator* gen, const jac_ast_block* block, jac_ast_variable_definition** variables
) {

    darray_foreach(block->statements, jac_ast_scope_statement, statement) {
        switch (statement->holds) {
        case JAC_AST_STATEMENT_RETURN:
            generate_statement_return(gen, statement->opt.ret, variables);
            break;
        case JAC_AST_BLOCK: generate_block(gen, statement->opt.block, variables); break;
        case JAC_AST_EXPRESSION_STATEMENT:
            generate_statement_expression(gen, statement->opt.expr_statement, variables);
            break;

        default: JAC_ASSERT(false, "unreachable.");
        }
    }
}

static void generate_extern_block(generator* gen, const jac_ast_extern_block* block) {
    darray_foreach(block->declarations, jac_ast_function_header, header)
        emit_noindent(gen, SECTION_TEXT, "extern %s\n", header->identifier->value);
}

static void generate_function_definition(generator* gen, jac_ast_function_definition* func_def) {
    emit_noindent(gen, SECTION_TEXT, "%s:\n", func_def->header->identifier->value);

    size_t argument_count = darray_count(func_def->header->parameters);
    size_t stack_space    = argument_count * 8;
    size_t aligned_stack  = (stack_space + 15) & ~15;

    push(gen, "rbp");
    emit(gen, SECTION_TEXT, "mov rbp, rsp\n");
    emit(gen, SECTION_TEXT, "sub rsp, %lu\n", aligned_stack + 8);

    for (size_t i = 0; i < darray_count(func_def->header->parameters); ++i) {
        jac_ast_variable_declaration* arg = func_def->header->parameters + i;
        emit(gen, SECTION_TEXT, "mov [rbp - %lu], %s\n", i * 8 + 8, argument_registers[i]);
        darray_push(func_def->arguments, arg->identifier);
    }

    jac_ast_variable_definition* variables = darray_new(jac_ast_variable_definition);

    generate_block(gen, func_def->block, &variables);

    emit_noindent(gen, SECTION_TEXT, ".ret:\n");
    emit(gen, SECTION_TEXT, "mov rsp, rbp\n");
    pop(gen, "rbp");
    emit(gen, SECTION_TEXT, "ret\n");
}

static void generate_unit_statement(generator* gen, const jac_ast_unit_statement* statement) {

    switch (statement->holds) {
    case JAC_AST_FUNCTION_DEFINITION:
        generate_function_definition(gen, statement->opt.func_def);
        break;
    case JAC_AST_EXTERN_BLOCK: generate_extern_block(gen, statement->opt.extern_block); break;

    default: JAC_ASSERT(false, "unreachable.");
    }
}

bool jac_generate_unit(jac_ast_unit* unit, char** source_out) {
    generator gen;
    init_generator(&gen);

    emit_noindent(&gen, SECTION_TEXT, "global _start\n");
    emit_noindent(&gen, SECTION_TEXT, "_start:\n");

    emit(&gen, SECTION_TEXT, "mov rdi, [rsp]\n");
    emit(&gen, SECTION_TEXT, "lea rsi, [rsp + 8]\n");
    emit(&gen, SECTION_TEXT, "sub rsp, 8\n");
    emit(&gen, SECTION_TEXT, "call main\n");

    emit(&gen, SECTION_TEXT, "mov rdi, rax\n");
    emit(&gen, SECTION_TEXT, "mov rax, 0x3c\n");
    emit(&gen, SECTION_TEXT, "syscall\n");

    darray_foreach(unit->statements, jac_ast_unit_statement, statement)
        generate_unit_statement(&gen, statement);

    size_t total_size = 0;
    for (size_t i = 0; i < END_SECTION; ++i) {
        if (!gen.sections[i]) continue;
        total_size += darray_count(gen.sections[i]);
    }

    char* concatenated = darray_new_reserved(char, total_size);
    for (size_t i = 0; i < END_SECTION; ++i) {
        if (!gen.sections[i]) continue;
        concatenated = concat(concatenated, gen.sections[i], darray_count(gen.sections[i]));
    }

    free_generator(&gen);
    *source_out = concatenated;
    return true;
}
