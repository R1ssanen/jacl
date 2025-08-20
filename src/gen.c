#include "gen.h"

#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "assert.h"
#include "ast.h"
#include "darray.h"
#include "stack.h"

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
    darray_t char* sections[END_SECTION];
} generator;

static char* concat(darray_t char* array, const char* str, size_t len) {
    size_t old_count = darray_count(array);
    array            = darray_grow_impl((size_t*)array, len);
    memcpy((void*)(array + old_count), str, len * sizeof(char));
    return array;
}

static void emit_noindent(generator*, section, const char*, ...);

static void
emit_impl(generator* gen, section section_, size_t indent, const char* fmt, va_list args) {
    char buffer[256];
    memset((void*)buffer, ' ', indent);
    size_t written = vsprintf(buffer + indent, fmt, args);

    if (!gen->sections[section_]) {
        gen->sections[section_] = darray_new(char);
        emit_noindent(gen, section_, "section %s\n", section_names[section_]);
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
}

static void free_generator(generator* gen) {
    for (size_t i = 0; i < END_SECTION; ++i) {
        if (!gen->sections[i]) continue;
        darray_free(gen->sections[i]);
    }
}

void push(generator* gen, const char* value) { emit(gen, SECTION_TEXT, "%s %s\n", "push", value); }

void pop(generator* gen, const char* register_) {
    emit(gen, SECTION_TEXT, "%s %s\n", "pop", register_);
}

/*
 * CODEGEN
 */

void generate_literal(generator* gen, const jac_token* literal) {
    // emit(gen, SECTION_TEXT, "push", literal->value, "\n");
    push(gen, literal->value);
}

void generate_expression(generator*, const jac_node_expression*);

void generate_statement_expression_call(
    generator* gen, const jac_node_statement_expression_call* call
) {

    for (size_t i = 0; i < darray_count(call->arguments); ++i) {
        generate_expression(gen, call->arguments + i);
        pop(gen, argument_registers[i]);
    }

    emit(gen, SECTION_TEXT, "%s %s\n", "call", call->identifier->value);
    push(gen, "rax");
}

void generate_statement_expression(
    generator* gen, const jac_node_statement_expression* statement_expr
) {

    switch (statement_expr->holds) {
    case JAC_NODE_STATEMENT_EXPR_CALL:
        generate_statement_expression_call(gen, statement_expr->opt.call);
        break;

    default: JAC_ASSERT(false, "unreachable.");
    }
}

void generate_identifier(generator* gen, const jac_token* identifier) { }

void generate_reference(generator* gen, const jac_token* reference) { }

void generate_expression(generator* gen, const jac_node_expression* expression) {

    switch (expression->holds) {
    case JAC_NODE_EXPRESSION_LITERAL: generate_literal(gen, expression->opt.literal); break;
    case JAC_NODE_STATEMENT_EXPR:
        generate_statement_expression(gen, expression->opt.statement_expr);
        break;
    case JAC_NODE_EXPRESSION_IDENTIFIER:
        generate_identifier(gen, expression->opt.identifier);
        break;
    case JAC_NODE_EXPRESSION_REF: generate_reference(gen, expression->opt.reference); break;

    default: JAC_ASSERT(false, "unreachable.");
    }
}

void generate_statement_return(generator* gen, const jac_node_statement_return* return_) {
    generate_expression(gen, return_->expression);
    pop(gen, "rax");
    emit(gen, SECTION_TEXT, "%s %s, %s\n", "mov", "rsp", "rbp");
    pop(gen, "rbp");
    emit(gen, SECTION_TEXT, "%s\n", "ret");
}

void generate_block(generator* gen, const jac_node_block* block) {

    darray_foreach(block->statements, jac_node_scope_statement, statement) {
        switch (statement->holds) {
        case JAC_NODE_STATEMENT_RETURN:
            generate_statement_return(gen, statement->opt.return_);
            break;
        case JAC_NODE_BLOCK: generate_block(gen, statement->opt.block); break;
        case JAC_NODE_STATEMENT_EXPR:
            generate_statement_expression(gen, statement->opt.expr_statement);
            break;

        default: JAC_ASSERT(false, "unreachable.");
        }
    }
}

void generate_function_declaration(generator* gen, const jac_node_function_declaration* func_decl) {
    emit_noindent(gen, SECTION_TEXT, "%s %s\n", "extern", func_decl->header->identifier->value);
}

void generate_function_definition(generator* gen, const jac_node_function_definition* func_def) {
    emit_noindent(gen, SECTION_TEXT, "%s:\n", func_def->header->identifier->value);

    size_t argument_count = darray_count(func_def->header->parameters);
    size_t stack_space    = argument_count * 8;
    size_t aligned_stack  = (stack_space + 15) & ~15;

    push(gen, "rbp");
    emit(gen, SECTION_TEXT, "%s %s, %s\n", "mov", "rbp", "rsp");
    emit(gen, SECTION_TEXT, "%s %s, %lu\n", "sub", "rsp", aligned_stack + 8);

    generate_block(gen, func_def->block);
}

void generate_unit_statement(generator* gen, const jac_node_unit_statement* statement) {

    switch (statement->holds) {
    case JAC_NODE_FUNCTION_DEFINITION:
        generate_function_definition(gen, statement->opt.func_def);
        break;
    case JAC_NODE_FUNCTION_DECLARATION:
        generate_function_declaration(gen, statement->opt.func_decl);
        break;

    default: JAC_ASSERT(false, "unreachable.");
    }
}

bool jac_generate_unit(jac_node_unit* unit, char** source_out) {
    generator gen;
    init_generator(&gen);

    emit_noindent(&gen, SECTION_TEXT, "%s %s\n", "global", "_start");
    emit_noindent(&gen, SECTION_TEXT, "%s:\n", "_start");

    emit(&gen, SECTION_TEXT, "%s %s, %s\n", "mov", "rdi", "[rsp]");
    emit(&gen, SECTION_TEXT, "%s %s, %s\n", "lea", "rsi", "[rsp + 8]");
    emit(&gen, SECTION_TEXT, "%s %s\n", "call", "main");

    emit(&gen, SECTION_TEXT, "%s %s, %s\n", "mov", "rdi", "rax");
    emit(&gen, SECTION_TEXT, "%s %s, %s\n", "mov", "rax", "0x3c");
    emit(&gen, SECTION_TEXT, "%s\n", "syscall");

    darray_foreach(unit->statements, jac_node_unit_statement, statement)
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
