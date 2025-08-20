#include "ast.h"

#include <stdarg.h>

#include "arena.h"
#include "assert.h"
#include "darray.h"
#include "error.h"
#include "lex.h"

/*
 * PARSER
 */

typedef struct parser {
    const jac_token*  tokens;
    const jac_token*  c;
    jac_memory_arena* arena;
} parser;

static bool             is_eof(const parser* parser) { return parser->c->kind == JAC_TOKEN_EOF; }

static const jac_token* consume(parser* parser) {
    JAC_ASSERT(!is_eof(parser), "unexpected EOF");
    return parser->c++;
}

static const jac_token* expect(parser* parser, jac_token_kind expected) {
    if (parser->c->kind != expected) return NULL;
    else
        return consume(parser);
}

/*
 * PARSING
 */

static bool parse_expression(parser*, jac_node_expression*, jac_error*);

static bool parse_statement_expression_call(
    parser* parser, jac_node_statement_expression_call* call, jac_error* error
) {

    const jac_token* identifier = expect(parser, JAC_TOKEN_IDENTIFIER);
    if (!identifier) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected identifier.");
        return false;
    }

    if (!expect(parser, JAC_TOKEN_LPAREN)) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected '('.");
        return false;
    }

    call->arguments = darray_new(jac_node_expression);

    jac_node_expression expression;
    if (parse_expression(parser, &expression, NULL)) {
        darray_push(call->arguments, expression);

        while (expect(parser, JAC_TOKEN_COMMA)) {
            if (!parse_expression(parser, &expression, NULL)) {
                JAC_SET_SYNTAX_ERROR(error, parser->c, "expected expression.");
                return false;
            }
            darray_push(call->arguments, expression);
        }
    }

    if (!expect(parser, JAC_TOKEN_RPAREN)) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected ')'.");
        return false;
    }

    call->identifier = identifier;
    return true;
}

static bool parse_statement_expression(
    parser* parser, jac_node_statement_expression* statement_expr, jac_error* error
) {

    switch (parser->c->kind) {
    case JAC_TOKEN_IDENTIFIER: {
        jac_node_statement_expression_call call;
        if (!parse_statement_expression_call(parser, &call, error)) return false;

        statement_expr->holds = JAC_NODE_STATEMENT_EXPR_CALL;
        statement_expr->opt.call =
            jac_arena_alloc(parser->arena, jac_node_statement_expression_call);
        *statement_expr->opt.call = call;
        return true;
    }

    default: return false;
    }
}

static bool parse_expression(parser* parser, jac_node_expression* expression, jac_error* error) {

    switch (parser->c->kind) {
    case JAC_TOKEN_LITERAL: {
        expression->holds       = JAC_NODE_EXPRESSION_LITERAL;
        expression->opt.literal = consume(parser);
        return true;
    }

    case JAC_TOKEN_AMPERSAND: {
        consume(parser);
        expression->holds         = JAC_NODE_EXPRESSION_REF;
        expression->opt.reference = expect(parser, JAC_TOKEN_IDENTIFIER);
        if (!expression->opt.reference) {
            JAC_SET_SYNTAX_ERROR(error, parser->c, "expected identifier.");
            return false;
        }
        return true;
    }

    case JAC_TOKEN_IDENTIFIER: {

        if (parser->c[1].kind == JAC_TOKEN_LPAREN) {
            jac_node_statement_expression statement_expr;
            if (!parse_statement_expression(parser, &statement_expr, error)) return false;

            expression->holds = JAC_NODE_STATEMENT_EXPR;
            expression->opt.statement_expr =
                jac_arena_alloc(parser->arena, jac_node_statement_expression);
            *expression->opt.statement_expr = statement_expr;
            return true;
        }

        else {
            expression->holds          = JAC_NODE_EXPRESSION_IDENTIFIER;
            expression->opt.identifier = consume(parser);
            return true;
        }
    }

    default: JAC_SET_SYNTAX_ERROR(error, parser->c, "invalid expression."); return false;
    }
}

static bool
parse_statement_return(parser* parser, jac_node_statement_return* return_, jac_error* error) {

    if (!expect(parser, JAC_TOKEN_RETURN)) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected 'return'.");
        return false;
    }

    jac_node_expression expression;
    if (!parse_expression(parser, &expression, error)) return false;

    return_->expression  = jac_arena_alloc(parser->arena, jac_node_expression);
    *return_->expression = expression;
    return true;
}

static bool parse_block(parser*, jac_node_block*, jac_error*);

static bool
parse_scope_statement(parser* parser, jac_node_scope_statement* statement, jac_error* error) {

    switch (parser->c->kind) {

    case JAC_TOKEN_RETURN: {
        jac_node_statement_return return_;
        if (!parse_statement_return(parser, &return_, error)) return false;

        if (!expect(parser, JAC_TOKEN_SEMICOLON)) {
            JAC_SET_SYNTAX_ERROR(error, parser->c, "expected ';'.");
            return false;
        }

        statement->holds        = JAC_NODE_STATEMENT_RETURN;
        statement->opt.return_  = jac_arena_alloc(parser->arena, jac_node_statement_return);
        *statement->opt.return_ = return_;
        return true;
    }

    case JAC_TOKEN_LBRACE: {
        jac_node_block block;
        if (!parse_block(parser, &block, error)) return false;

        statement->holds      = JAC_NODE_BLOCK;
        statement->opt.block  = jac_arena_alloc(parser->arena, jac_node_block);
        *statement->opt.block = block;
        return true;
    }

    case JAC_TOKEN_IDENTIFIER: {
        jac_node_statement_expression statement_expr;
        if (!parse_statement_expression(parser, &statement_expr, error)) return false;

        if (!expect(parser, JAC_TOKEN_SEMICOLON)) {
            JAC_SET_SYNTAX_ERROR(error, parser->c, "expected ';'.");
            return false;
        }

        statement->holds = JAC_NODE_STATEMENT_EXPR;
        statement->opt.expr_statement =
            jac_arena_alloc(parser->arena, jac_node_statement_expression);
        *statement->opt.expr_statement = statement_expr;
        return true;
    }

    default: JAC_SET_SYNTAX_ERROR(error, parser->c, "invalid scope statement."); return false;
    }
}

static bool parse_block(parser* parser, jac_node_block* block, jac_error* error) {

    if (!expect(parser, JAC_TOKEN_LBRACE)) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected '{'.");
        return false;
    }

    block->statements = darray_new(jac_node_scope_statement);

    while (!is_eof(parser) && (parser->c->kind != JAC_TOKEN_RBRACE)) {
        jac_node_scope_statement statement;
        if (!parse_scope_statement(parser, &statement, error)) return false;

        darray_push(block->statements, statement);
    }

    if (!expect(parser, JAC_TOKEN_RBRACE)) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected '}'.");
        return false;
    }

    return true;
}

static bool parse_type(parser* parser, jac_node_type* type, jac_error* error) {

    switch (parser->c->kind) {
    case JAC_TOKEN_STAR: {
        consume(parser);

        jac_node_type pointer_type;
        if (!parse_type(parser, &pointer_type, error)) return false;

        type->holds        = JAC_NODE_TYPE_POINTER;
        type->opt.pointer  = jac_arena_alloc(parser->arena, jac_node_type);
        *type->opt.pointer = pointer_type;
        return true;
    }

    case JAC_TOKEN_UINT8:
    case JAC_TOKEN_INT32:
        type->holds      = JAC_NODE_TYPE_NORMAL;
        type->opt.normal = consume(parser);
        return true;

    default: JAC_SET_SYNTAX_ERROR(error, parser->c, "expected type."); return false;
    }
}

static bool parse_variable_declaration(
    parser* parser, jac_node_variable_declaration* variable_decl, jac_error* error
) {

    jac_node_type type;
    if (!parse_type(parser, &type, error)) return false;

    if (!expect(parser, JAC_TOKEN_COLON)) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected ':'.");
        return false;
    }

    variable_decl->identifier = expect(parser, JAC_TOKEN_IDENTIFIER);
    if (!variable_decl->identifier) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected identifier.");
        return false;
    }

    variable_decl->type  = jac_arena_alloc(parser->arena, jac_node_type);
    *variable_decl->type = type;
    return true;
}

static bool
parse_function_header(parser* parser, jac_node_function_header* header, jac_error* error) {

    if (!expect(parser, JAC_TOKEN_FUNC)) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected 'func'.");
        return false;
    }

    header->identifier = expect(parser, JAC_TOKEN_IDENTIFIER);
    if (!header->identifier) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected identifier.");
        return false;
    }

    if (!expect(parser, JAC_TOKEN_LPAREN)) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected '('.");
        return false;
    }

    header->parameters = darray_new(jac_node_variable_declaration);

    jac_node_variable_declaration variable_decl;
    if (parse_variable_declaration(parser, &variable_decl, NULL)) {
        darray_push(header->parameters, variable_decl);

        while (expect(parser, JAC_TOKEN_COMMA)) {
            if (!parse_variable_declaration(parser, &variable_decl, error)) return false;
            darray_push(header->parameters, variable_decl);
        }
    }

    if (!expect(parser, JAC_TOKEN_RPAREN)) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected ')'.");
        return false;
    }

    if (!expect(parser, JAC_TOKEN_MINUS)) {
        header->return_type = NULL;
        return true;
    }

    if (!expect(parser, JAC_TOKEN_GT)) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected '>'.");
        return false;
    }

    jac_node_type return_type;
    if (!parse_type(parser, &return_type, error)) return false;

    header->return_type  = jac_arena_alloc(parser->arena, jac_node_type);
    *header->return_type = return_type;
    return true;
}

static bool parse_function_declaration(
    parser* parser, jac_node_function_declaration* func_decl, jac_error* error
) {

    if (!expect(parser, JAC_TOKEN_EXTERN)) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected 'extern'.");
        return false;
    }

    jac_node_function_header header;
    if (!parse_function_header(parser, &header, error)) return false;

    if (!expect(parser, JAC_TOKEN_SEMICOLON)) {
        JAC_SET_SYNTAX_ERROR(error, parser->c, "expected ';'.");
        return false;
    }

    func_decl->header  = jac_arena_alloc(parser->arena, jac_node_function_header);
    *func_decl->header = header;
    return true;
}

static bool parse_function_definition(
    parser* parser, jac_node_function_definition* func_def, jac_error* error
) {

    jac_node_function_header header;
    if (!parse_function_header(parser, &header, error)) return false;

    jac_node_block block;
    if (!parse_block(parser, &block, error)) return false;

    func_def->header  = jac_arena_alloc(parser->arena, jac_node_function_header);
    *func_def->header = header;

    func_def->block   = jac_arena_alloc(parser->arena, jac_node_block);
    *func_def->block  = block;

    return true;
}

static bool
parse_unit_statement(parser* parser, jac_node_unit_statement* statement, jac_error* error) {

    switch (parser->c->kind) {
    case JAC_TOKEN_FUNC: {
        jac_node_function_definition func_def;
        if (!parse_function_definition(parser, &func_def, error)) return false;

        statement->holds         = JAC_NODE_FUNCTION_DEFINITION;
        statement->opt.func_def  = jac_arena_alloc(parser->arena, jac_node_function_definition);
        *statement->opt.func_def = func_def;
        return true;
    }

    case JAC_TOKEN_EXTERN: {
        jac_node_function_declaration func_decl;
        if (!parse_function_declaration(parser, &func_decl, error)) return false;

        statement->holds          = JAC_NODE_FUNCTION_DECLARATION;
        statement->opt.func_decl  = jac_arena_alloc(parser->arena, jac_node_function_declaration);
        *statement->opt.func_decl = func_decl;
        return true;
    }

    default: JAC_SET_SYNTAX_ERROR(error, parser->c, "invalid unit statement."); return false;
    }
}

static void skip_statement(parser* parser) {

    while (!is_eof(parser)) {
        const jac_token* token = consume(parser);
        if (token->kind == JAC_TOKEN_SEMICOLON) return;
        if (token->kind == JAC_TOKEN_RBRACE) return;
    }
}

bool jac_parse_unit(jac_memory_arena* arena, darray_t jac_token* tokens, jac_node_unit* unit) {

    parser parser    = { .tokens = tokens, .c = tokens, .arena = arena };

    unit->statements = darray_new(jac_node_unit_statement);
    jac_error error  = { 0 };

    while (!is_eof(&parser)) {

        jac_node_unit_statement statement;
        if (parse_unit_statement(&parser, &statement, &error)) {
            darray_push(unit->statements, statement);
        }

        else {
            fprintf(
                stderr, "[%lu, %lu] (%s) syntax error; %s\n", error.opt.token->line,
                error.opt.token->col, error.opt.token->value, error.msg
            );
            skip_statement(&parser);
        }
    }

    return error.msg == NULL;
}

/*
 * CLEANUP
 */

static void free_expression(jac_node_expression*);

static void free_statement_expression_call(jac_node_statement_expression_call* call) {
    darray_foreach(call->arguments, jac_node_expression, expression) free_expression(expression);
    darray_free(call->arguments);
}

static void free_statement_expression(jac_node_statement_expression* statement_expr) {
    switch (statement_expr->holds) {
    case JAC_NODE_STATEMENT_EXPR_CALL:
        free_statement_expression_call(statement_expr->opt.call);
        break;

    default: JAC_ASSERT(false, "unreachable.");
    }
}

static void free_expression(jac_node_expression* expression) {
    switch (expression->holds) {
    case JAC_NODE_STATEMENT_EXPR: free_statement_expression(expression->opt.statement_expr); break;
    case JAC_NODE_EXPRESSION_LITERAL: break;
    case JAC_NODE_EXPRESSION_IDENTIFIER: break;
    case JAC_NODE_EXPRESSION_REF: break;

    default: JAC_ASSERT(false, "unreachable.");
    }
}

static void free_return(jac_node_statement_return* return_) {
    free_expression(return_->expression);
}

static void free_block(jac_node_block* block) {
    if (!block->statements) return;

    darray_foreach(block->statements, jac_node_scope_statement, statement) {
        switch (statement->holds) {
        case JAC_NODE_STATEMENT_RETURN: free_return(statement->opt.return_); break;
        case JAC_NODE_STATEMENT_EXPR:
            free_statement_expression(statement->opt.expr_statement);
            break;
        case JAC_NODE_BLOCK:
            if (!statement->opt.block) break;
            free_block(statement->opt.block);
            break;

        default: JAC_ASSERT(false, "unreachable.");
        }
    }

    darray_free(block->statements);
}

static void free_function_header(jac_node_function_header* header) {
    darray_free(header->parameters);
}

static void free_function_definition(jac_node_function_definition* func_def) {
    if (!func_def->block) return;
    free_function_header(func_def->header);
    free_block(func_def->block);
}

static void free_function_declaration(jac_node_function_declaration* func_decl) {
    free_function_header(func_decl->header);
}

void jac_free_unit(jac_node_unit* unit) {
    if (!unit->statements) return;

    darray_foreach(unit->statements, jac_node_unit_statement, statement) {
        switch (statement->holds) {
        case JAC_NODE_FUNCTION_DEFINITION: free_function_definition(statement->opt.func_def); break;
        case JAC_NODE_FUNCTION_DECLARATION:
            free_function_declaration(statement->opt.func_decl);
            break;

        default: JAC_ASSERT(false, "unreachable.");
        }
    }

    darray_free(unit->statements);
}

/*
 * DEBUGGING
 */

static void indented(int level, const char* msg) { printf("%*s%s\n", level, " ", msg); }

static void indented_v(int level, const char* fmt, ...) {
    printf("%*s", level, " ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    putchar('\n');
}

static void print_token(const jac_token* token, const char* label, int level) {
    indented_v(level, "%s: %s", label, token->value);
}

static void print_expression(const jac_node_expression*, int, int);

static void print_statement_expression_call(
    const jac_node_statement_expression_call* call, int level, int indent
) {
    indented(level, "statement_expression_call");
    print_token(call->identifier, "id", level + indent);

    darray_foreach(call->arguments, jac_node_expression, expression)
        print_expression(expression, level + indent, indent);
}

static void print_statement_expression(
    const jac_node_statement_expression* statement_expr, int level, int indent
) {
    indented(level, "statement_expression");

    switch (statement_expr->holds) {
    case JAC_NODE_STATEMENT_EXPR_CALL:
        print_statement_expression_call(statement_expr->opt.call, level + indent, indent);
        break;

    default: JAC_ASSERT(false, "unreachable.");
    }
}

static void print_expression(const jac_node_expression* expression, int level, int indent) {
    indented(level, "expression");

    switch (expression->holds) {
    case JAC_NODE_EXPRESSION_LITERAL:
        print_token(expression->opt.literal, "literal", level + indent);
        break;
    case JAC_NODE_STATEMENT_EXPR:
        print_statement_expression(expression->opt.statement_expr, level + indent, indent);
        break;
    case JAC_NODE_EXPRESSION_IDENTIFIER:
        print_token(expression->opt.identifier, "id", level + indent);
        break;
    case JAC_NODE_EXPRESSION_REF:
        print_token(expression->opt.reference, "ref", level + indent);
        break;

    default: JAC_ASSERT(false, "unreachable.");
    }
}

static void
print_statement_return(const jac_node_statement_return* return_, int level, int indent) {
    indented(level, "return");
    print_expression(return_->expression, level + indent, indent);
}

static void print_block(const jac_node_block* block, int level, int indent) {
    indented(level, "block");

    darray_foreach(block->statements, jac_node_scope_statement, statement) {
        switch (statement->holds) {
        case JAC_NODE_STATEMENT_RETURN:
            print_statement_return(statement->opt.return_, level + indent, indent);
            break;
        case JAC_NODE_BLOCK: print_block(statement->opt.block, level + indent, indent); break;
        case JAC_NODE_STATEMENT_EXPR:
            print_statement_expression(statement->opt.expr_statement, level + indent, indent);
            break;

        default: JAC_ASSERT(false, "unreachable.");
        }
    }
}

static void print_type(const jac_node_type* type, int level, int indent) {

    if (!type) {
        indented(level, "none_type");
        return;
    }

    switch (type->holds) {
    case JAC_NODE_TYPE_POINTER:
        indented(level, "pointer_type");
        print_type(type->opt.pointer, level + indent, indent);
        break;
    case JAC_NODE_TYPE_NORMAL: print_token(type->opt.normal, "normal_type", level); break;

    default: JAC_ASSERT(false, "unreachable.");
    }
}

static void print_variable_declaration(
    const jac_node_variable_declaration* variable_decl, int level, int indent
) {
    indented(level, "variable_declaration");

    print_token(variable_decl->identifier, "id", level + indent);
    print_type(variable_decl->type, level + indent, indent);
}

static void print_function_header(const jac_node_function_header* header, int level, int indent) {
    indented(level, "function_header");
    print_token(header->identifier, "id", level + indent);
    print_type(header->return_type, level + indent, indent);

    darray_foreach(header->parameters, jac_node_variable_declaration, variable_decl)
        print_variable_declaration(variable_decl, level + indent, indent);
}

static void
print_function_declaration(const jac_node_function_declaration* func_decl, int level, int indent) {
    indented(level, "function_declaration");
    print_function_header(func_decl->header, level + indent, indent);
}

static void
print_function_definition(const jac_node_function_definition* func_def, int level, int indent) {
    indented(level, "function_definition");
    print_function_header(func_def->header, level + indent, indent);
    print_block(func_def->block, level + indent, indent);
}

void jac_print_unit(const jac_node_unit* unit, int indent) {
    printf("unit\n");

    darray_foreach(unit->statements, jac_node_unit_statement, statement) {
        switch (statement->holds) {
        case JAC_NODE_FUNCTION_DEFINITION:
            print_function_definition(statement->opt.func_def, indent, indent);
            break;
        case JAC_NODE_FUNCTION_DECLARATION:
            print_function_declaration(statement->opt.func_decl, indent, indent);
            break;

        default: JAC_ASSERT(false, "unreachable.");
        }
    }
}
