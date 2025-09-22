#include "ast.h"

#include <stdarg.h>

#include "arena.h"
#include "assert.h"
#include "darray.h"
#include "diagnostics.h"
#include "lex.h"
#include "symbol.h"
#include "type.h"

/*
 * PARSER
 */

typedef struct parser parser;

struct parser
{
    const jac_token *tokens;
    const jac_token *c;
    jac_memory_arena *arena;
};

static bool is_eof(const parser *parser)
{
    return parser->c->kind == JAC_TOKEN_EOF;
}

static const jac_token *consume(parser *parser)
{
    JAC_ASSERT(!is_eof(parser), "unexpected EOF");
    return parser->c++;
}

static const jac_token *expect(parser *parser, enum jac_token_kind expected)
{
    if (parser->c->kind != expected)
        return NULL;
    return consume(parser);
}

/*
 * PARSING
 */

static bool parse_expression(parser *, bool, jac_ast_expression *);

static bool parse_statement_expression_call(parser *parser, bool required, jac_ast_function_call *call)
{

    const jac_token *identifier = expect(parser, JAC_TOKEN_IDENTIFIER);
    if (!identifier)
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected identifier, got '%s'.", parser->c->value);
        return false;
    }

    if (!expect(parser, JAC_TOKEN_LPAREN))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected '(', got '%s'.", parser->c->value);
        return false;
    }

    call->arguments = darray_new(jac_ast_expression);

    jac_ast_expression expression;
    if (parse_expression(parser, false, &expression))
    {
        darray_push(call->arguments, expression);

        while (expect(parser, JAC_TOKEN_COMMA))
        {
            if (!parse_expression(parser, required, &expression))
            {
                if (required)
                {
                    jac_print_diagnostic(parser->c, "expected argument expression, got '%s'.", parser->c->value);
                }
                return false;
            }
            darray_push(call->arguments, expression);
        }
    }

    if (!expect(parser, JAC_TOKEN_RPAREN))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected ')', got '%s'.", parser->c->value);
        return false;
    }

    call->identifier = identifier;
    return true;
}

static bool parse_variable_declaration(parser *, bool, jac_symbol *);

static bool parse_variable_definition(parser *parser, bool required, jac_ast_variable_definition *var_def)
{

    if (!parse_variable_declaration(parser, required, &var_def->declaration))
        return false;

    if (!expect(parser, JAC_TOKEN_COLON))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected ':', got '%s'.", parser->c->value);
        return false;
    }

    if (!expect(parser, JAC_TOKEN_EQUALS))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected '=', got '%s'.", parser->c->value);
        return false;
    }

    jac_ast_expression expression;
    if (!parse_expression(parser, required, &expression))
        return false;

    var_def->expression = jac_arena_alloc(parser->arena, jac_ast_expression);
    *var_def->expression = expression;
    return true;
}

static bool parse_assignment(parser *parser, bool required, jac_ast_assignment *assignment)
{

    assignment->identifier = expect(parser, JAC_TOKEN_IDENTIFIER);
    if (!assignment->identifier)
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected identifier, got '%s'.", parser->c->value);
        return false;
    }

    if (!expect(parser, JAC_TOKEN_COLON))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected ':', got '%s'.", parser->c->value);
        return false;
    }

    if (!expect(parser, JAC_TOKEN_EQUALS))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected '=', got '%s'.", parser->c->value);
        return false;
    }

    jac_ast_expression expression;
    if (!parse_expression(parser, required, &expression))
        return false;

    assignment->expression = jac_arena_alloc(parser->arena, jac_ast_expression);
    *assignment->expression = expression;
    return true;
}

static bool parse_statement_expression(parser *parser, bool required, jac_ast_expression_statement *expr_statement)
{

    switch (parser->c->kind)
    {
    case JAC_TOKEN_IDENTIFIER: {

        if (parser->c[1].kind == JAC_TOKEN_LPAREN)
        {
            jac_ast_function_call call;
            if (!parse_statement_expression_call(parser, required, &call))
                return false;

            expr_statement->holds = JAC_AST_FUNCTION_CALL;
            expr_statement->opt.call = jac_arena_alloc(parser->arena, jac_ast_function_call);
            *expr_statement->opt.call = call;
            return true;
        }

        else
        {
            jac_ast_assignment assignment;
            if (!parse_assignment(parser, required, &assignment))
                return false;

            expr_statement->holds = JAC_AST_ASSIGNMENT;
            expr_statement->opt.assignment = jac_arena_alloc(parser->arena, jac_ast_assignment);
            *expr_statement->opt.assignment = assignment;
            return true;
        }
    }

    default: {
        jac_ast_variable_definition var_def;
        if (parse_variable_definition(parser, required, &var_def))
        {
            expr_statement->holds = JAC_AST_VARIABLE_DEFINITION;
            expr_statement->opt.var_def = jac_arena_alloc(parser->arena, jac_ast_variable_definition);
            *expr_statement->opt.var_def = var_def;
            return true;
        }

        if (required)
            jac_print_diagnostic(parser->c, "invalid statement expression.");
        return false;
    }
    }
}

static bool parse_expression(parser *parser, bool required, jac_ast_expression *expression)
{

    switch (parser->c->kind)
    {
    case JAC_TOKEN_STR_LITERAL:
    case JAC_TOKEN_INT_LITERAL: {
        expression->holds = JAC_AST_EXPRESSION_LITERAL;
        expression->opt.literal = consume(parser);
        return true;
    }

    case JAC_TOKEN_AMPERSAND: {
        consume(parser);
        expression->holds = JAC_AST_EXPRESSION_ADDROF;
        expression->opt.addrof = expect(parser, JAC_TOKEN_IDENTIFIER);
        if (!expression->opt.addrof)
        {
            if (required)
                jac_print_diagnostic(parser->c, "expected identifier, got '%s'.", parser->c->value);
            return false;
        }
        return true;
    }

    case JAC_TOKEN_IDENTIFIER: {
        if (parser->c[1].kind == JAC_TOKEN_LPAREN)
        {
            jac_ast_expression_statement expr_statement;
            if (!parse_statement_expression(parser, required, &expr_statement))
                return false;

            expression->holds = JAC_AST_EXPRESSION_STATEMENT;
            expression->opt.expr_statement = jac_arena_alloc(parser->arena, jac_ast_expression_statement);
            *expression->opt.expr_statement = expr_statement;
            return true;
        }

        else
        {
            expression->holds = JAC_AST_EXPRESSION_IDENTIFIER;
            expression->opt.identifier = consume(parser);
            return true;
        }
    }

    default:
        if (required)
            jac_print_diagnostic(parser->c, "invalid expression.");
        return false;
    }
}

static bool parse_statement_return(parser *parser, bool required, jac_ast_statement_return *ret)
{

    if (!expect(parser, JAC_TOKEN_RETURN))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected 'return', got '%s'.", parser->c->value);
        return false;
    }

    jac_ast_expression expression;
    if (!parse_expression(parser, false, &expression))
    {
        ret->expression = NULL;
        return true;
    }

    ret->expression = jac_arena_alloc(parser->arena, jac_ast_expression);
    *ret->expression = expression;
    return true;
}

static bool parse_block(parser *, bool, jac_ast_block *);

static bool parse_scope_statement(parser *parser, bool required, jac_ast_scope_statement *statement)
{

    switch (parser->c->kind)
    {

    case JAC_TOKEN_RETURN: {
        jac_ast_statement_return ret;
        if (!parse_statement_return(parser, required, &ret))
            return false;

        if (!expect(parser, JAC_TOKEN_SEMICOLON))
        {
            if (required)
                jac_print_diagnostic(parser->c, "expected ';', got '%s'.", parser->c->value);
            return false;
        }

        statement->holds = JAC_AST_STATEMENT_RETURN;
        statement->opt.ret = jac_arena_alloc(parser->arena, jac_ast_statement_return);
        *statement->opt.ret = ret;
        return true;
    }

    case JAC_TOKEN_LBRACE: {
        if (!parse_block(parser, required, &statement->opt.block))
            return false;
        statement->holds = JAC_AST_BLOCK;
        return true;
    }

    default: {
        jac_ast_expression_statement expr_statement;
        if (parse_statement_expression(parser, required, &expr_statement))
        {
            if (!expect(parser, JAC_TOKEN_SEMICOLON))
            {
                if (required)
                    jac_print_diagnostic(parser->c, "expected ';', got '%s'.", parser->c->value);
                return false;
            }

            statement->holds = JAC_AST_EXPRESSION_STATEMENT;
            statement->opt.expr_statement = jac_arena_alloc(parser->arena, jac_ast_expression_statement);
            *statement->opt.expr_statement = expr_statement;
            return true;
        }

        if (required)
            jac_print_diagnostic(parser->c, "invalid scope statement.", parser->c->value);
        return false;
    }
    }
}

static bool parse_block(parser *parser, bool required, jac_ast_block *block)
{

    if (!expect(parser, JAC_TOKEN_LBRACE))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected '{', got '%s'.", parser->c->value);
        return false;
    }

    *block = darray_new(jac_ast_scope_statement);

    while (!is_eof(parser) && (parser->c->kind != JAC_TOKEN_RBRACE))
    {
        jac_ast_scope_statement statement;
        if (!parse_scope_statement(parser, required, &statement))
            return false;
        darray_push(*block, statement);
    }

    if (!expect(parser, JAC_TOKEN_RBRACE))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected '}', got '%s'.", parser->c->value);
        return false;
    }

    return true;
}

static bool parse_type(parser *parser, bool required, jac_type *type)
{
    type->indirection = 0;

    while (parser->c->kind == JAC_TOKEN_STAR)
    {
        type->indirection += 1;
        consume(parser);
    }

    switch (parser->c->kind)
    {
    case JAC_TOKEN_IDENTIFIER:
        type->kind = JAC_TYPE_CUSTOM;
        break;
        // case JAC_TOKEN_INT8: type->kind = JAC_TYPE_INT8; break;
    case JAC_TOKEN_UINT8:
        type->kind = JAC_TYPE_UINT8;
        break;
    case JAC_TOKEN_INT32:
        type->kind = JAC_TYPE_INT32;
        break;
        // case JAC_TOKEN_UINT32: type->kind = JAC_TYPE_UINT32; break;
        // case JAC_TOKEN_INT64: type->kind = JAC_TYPE_INT64; break;
        // case JAC_TOKEN_UINT64: type->kind = JAC_TYPE_UINT64; break;

    default:
        if (required)
            jac_print_diagnostic(parser->c, "expected type, got '%s'.", parser->c->value);
        return false;
    }

    type->token = consume(parser);
    return true;
}

static bool parse_variable_declaration(parser *parser, bool required, jac_symbol *declaration)
{

    if (!parse_type(parser, required, &declaration->type))
        return false;

    if (!expect(parser, JAC_TOKEN_COLON))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected ':', got '%s'.", parser->c->value);
        return false;
    }

    declaration->identifier = expect(parser, JAC_TOKEN_IDENTIFIER);
    if (!declaration->identifier)
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected identifier, got '%s'.", parser->c->value);
        return false;
    }

    return true;
}

static bool parse_function_header(parser *parser, bool required, jac_function *header)
{

    if (!expect(parser, JAC_TOKEN_FUNC))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected 'func', got '%s'.", parser->c->value);
        return false;
    }

    header->identifier = expect(parser, JAC_TOKEN_IDENTIFIER);
    if (!header->identifier)
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected identifier, got '%s'.", parser->c->value);
        return false;
    }

    header->args = darray_new(jac_symbol);
    if (expect(parser, JAC_TOKEN_LPAREN))
    {

        jac_symbol arg;
        if (parse_variable_declaration(parser, false, &arg))
        {
            darray_push(header->args, arg);

            while (expect(parser, JAC_TOKEN_COMMA))
            {
                if (!parse_variable_declaration(parser, required, &arg))
                    return false;
                darray_push(header->args, arg);
            }
        }

        if (!expect(parser, JAC_TOKEN_RPAREN))
        {
            if (required)
                jac_print_diagnostic(parser->c, "expected ')', got '%s'.", parser->c->value);
            return false;
        }
    }

    header->mangled = jac_mangle_function_name(header->identifier, header->args);

    if (!expect(parser, JAC_TOKEN_MINUS))
    {
        header->ret_type.kind = JAC_TYPE_NONE;
        return true;
    }

    if (!expect(parser, JAC_TOKEN_GT))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected '>', got '%s'.", parser->c->value);
        return false;
    }

    return parse_type(parser, true, &header->ret_type);
}

static bool parse_extern_block(parser *parser, bool required, jac_ast_extern_block *block)
{

    if (!expect(parser, JAC_TOKEN_EXTERN))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected 'extern', got '%s'.", parser->c->value);
        return false;
    }

    if (!expect(parser, JAC_TOKEN_LBRACE))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected '{', got '%s'.", parser->c->value);
        return false;
    }

    *block = darray_new(jac_function);
    while ((parser->c->kind != JAC_TOKEN_EOF) && (parser->c->kind != JAC_TOKEN_RBRACE))
    {
        jac_function header;
        if (!parse_function_header(parser, true, &header))
            return false;

        if (!expect(parser, JAC_TOKEN_SEMICOLON))
        {
            if (required)
                jac_print_diagnostic(parser->c, "expected ';', got '%s'.", parser->c->value);
            return false;
        }

        darray_push(*block, header);
    }

    if (!expect(parser, JAC_TOKEN_RBRACE))
    {
        if (required)
            jac_print_diagnostic(parser->c, "expected '}', got '%s'.", parser->c->value);
        return false;
    }

    return true;
}

static bool parse_function_definition(parser *parser, bool required, jac_ast_function_definition *func_def)
{
    if (!parse_function_header(parser, required, &func_def->header))
        return false;
    if (!parse_block(parser, true, &func_def->block))
        return false;
    return true;
}

static bool parse_unit_statement(parser *parser, jac_ast_unit_statement *statement)
{

    switch (parser->c->kind)
    {
    case JAC_TOKEN_FUNC: {
        jac_ast_function_definition func_def;
        if (!parse_function_definition(parser, true, &func_def))
            return false;

        statement->holds = JAC_AST_FUNCTION_DEFINITION;
        statement->opt.func_def = jac_arena_alloc(parser->arena, jac_ast_function_definition);
        *statement->opt.func_def = func_def;
        return true;
    }

    case JAC_TOKEN_EXTERN: {
        if (!parse_extern_block(parser, true, &statement->opt.extern_block))
            return false;
        statement->holds = JAC_AST_EXTERN_BLOCK;
        return true;
    }

    default:
        jac_print_diagnostic(parser->c, "invalid unit statement.");
        return false;
    }
}

static void skip_statement(parser *parser)
{

    while (!is_eof(parser))
    {
        const jac_token *token = consume(parser);
        if (token->kind == JAC_TOKEN_SEMICOLON)
        {
            consume(parser);
            return;
        }
        if (token->kind == JAC_TOKEN_RBRACE)
        {
            consume(parser);
            return;
        }
    }
}

bool jac_parse_unit(jac_memory_arena *arena, darray_t jac_token *tokens, jac_ast_unit *unit)
{

    parser parser = {.tokens = tokens, .c = tokens, .arena = arena};
    *unit = darray_new(jac_ast_unit_statement);
    bool success = true;

    while (!is_eof(&parser))
    {

        jac_ast_unit_statement statement;
        if (parse_unit_statement(&parser, &statement))
        {
            darray_push(*unit, statement);
        }
        else
        {
            success = false;
            skip_statement(&parser);
        }
    }

    if (!success)
    {
        jac_free_unit(*unit);
        return false;
    }

    return true;
}

/*
 * CLEANUP
 */

static void free_expression(jac_ast_expression *);

static void free_statement_expression_call(jac_ast_function_call *call)
{
    darray_foreach(call->arguments, jac_ast_expression, expression) free_expression(expression);
    darray_free(call->arguments);
}

static void free_variable_definition(jac_ast_variable_definition *var_def)
{
    free_expression(var_def->expression);
}

static void free_assignment(jac_ast_assignment *assignment)
{
    free_expression(assignment->expression);
}

static void free_statement_expression(jac_ast_expression_statement *expr_statement)
{
    switch (expr_statement->holds)
    {
    case JAC_AST_FUNCTION_CALL:
        free_statement_expression_call(expr_statement->opt.call);
        break;
    case JAC_AST_VARIABLE_DEFINITION:
        free_variable_definition(expr_statement->opt.var_def);
        break;
    case JAC_AST_ASSIGNMENT:
        free_assignment(expr_statement->opt.assignment);
        break;

    default:
        JAC_ASSERT(false, "unreachable.");
    }
}

static void free_expression(jac_ast_expression *expression)
{
    switch (expression->holds)
    {
    case JAC_AST_EXPRESSION_STATEMENT:
        free_statement_expression(expression->opt.expr_statement);
        break;
    case JAC_AST_EXPRESSION_LITERAL:
        break;
    case JAC_AST_EXPRESSION_IDENTIFIER:
        break;
    case JAC_AST_EXPRESSION_ADDROF:
        break;

    default:
        JAC_ASSERT(false, "unreachable.");
    }
}

static void free_return(jac_ast_statement_return *ret)
{
    if (ret->expression)
        free_expression(ret->expression);
}

static void free_block(jac_ast_block block)
{

    darray_foreach(block, jac_ast_scope_statement, statement)
    {
        switch (statement->holds)
        {
        case JAC_AST_STATEMENT_RETURN:
            free_return(statement->opt.ret);
            break;
        case JAC_AST_EXPRESSION_STATEMENT:
            free_statement_expression(statement->opt.expr_statement);
            break;
        case JAC_AST_BLOCK:
            if (!statement->opt.block)
                break;
            free_block(statement->opt.block);
            break;

        default:
            JAC_ASSERT(false, "unreachable.");
        }
    }

    darray_free(block);
}

static void free_function_header(jac_function *header)
{
    darray_free(header->args);
}

static void free_function_definition(jac_ast_function_definition *func_def)
{
    if (!func_def->block)
        return;
    free_function_header(&func_def->header);
    free_block(func_def->block);
}

static void free_extern_block(jac_ast_extern_block block)
{
    darray_foreach(block, jac_function, header) free_function_header(header);
    darray_free(block);
}

void jac_free_unit(jac_ast_unit unit)
{
    // if (!unit->statements) return;

    darray_foreach(unit, jac_ast_unit_statement, statement)
    {
        switch (statement->holds)
        {
        case JAC_AST_FUNCTION_DEFINITION:
            free_function_definition(statement->opt.func_def);
            break;
        case JAC_AST_EXTERN_BLOCK:
            free_extern_block(statement->opt.extern_block);
            break;

        default:
            JAC_ASSERT(false, "unreachable.");
        }
    }

    darray_free(unit);
}

/*
 * DEBUGGING
 */

static void indented(int level, const char *msg)
{
    printf("%*s%s\n", level, " ", msg);
}

static void indented_v(int level, const char *fmt, ...)
{
    printf("%*s", level, " ");
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    putchar('\n');
}

static void print_token(const jac_token *token, const char *label, int level)
{
    indented_v(level, "%s: %s", label, token->value);
}

static void print_expression(const jac_ast_expression *, int, int);

static void print_statement_expression_call(const jac_ast_function_call *call, int level, int indent)
{
    indented(level, "function_call");
    print_token(call->identifier, "id", level + indent);

    darray_foreach(call->arguments, jac_ast_expression, expression)
        print_expression(expression, level + indent, indent);
}

static void print_variable_declaration(const jac_symbol *, int, int);

static void print_variable_definition(const jac_ast_variable_definition *var_def, int level, int indent)
{
    indented(level, "variable_definition");
    print_variable_declaration(&var_def->declaration, level + indent, indent);
    print_expression(var_def->expression, level + indent, indent);
}

static void print_assignment(const jac_ast_assignment *assignment, int level, int indent)
{
    indented(level, "assignment");
    print_token(assignment->identifier, "id", level + indent);
    print_expression(assignment->expression, level + indent, indent);
}

static void print_statement_expression(const jac_ast_expression_statement *expr_statement, int level, int indent)
{
    indented(level, "expression_statement");

    switch (expr_statement->holds)
    {
    case JAC_AST_FUNCTION_CALL:
        print_statement_expression_call(expr_statement->opt.call, level + indent, indent);
        break;
    case JAC_AST_VARIABLE_DEFINITION:
        print_variable_definition(expr_statement->opt.var_def, level + indent, indent);
        break;
    case JAC_AST_ASSIGNMENT:
        print_assignment(expr_statement->opt.assignment, level + indent, indent);
        break;

    default:
        JAC_ASSERT(false, "unreachable.");
    }
}

static void print_expression(const jac_ast_expression *expression, int level, int indent)
{
    indented(level, "expression");

    switch (expression->holds)
    {
    case JAC_AST_EXPRESSION_LITERAL:
        print_token(expression->opt.literal, "literal", level + indent);
        break;
    case JAC_AST_EXPRESSION_STATEMENT:
        print_statement_expression(expression->opt.expr_statement, level + indent, indent);
        break;
    case JAC_AST_EXPRESSION_IDENTIFIER:
        print_token(expression->opt.identifier, "id", level + indent);
        break;
    case JAC_AST_EXPRESSION_ADDROF:
        print_token(expression->opt.addrof, "ref", level + indent);
        break;

    default:
        JAC_ASSERT(false, "unreachable.");
    }
}

static void print_statement_return(const jac_ast_statement_return *ret, int level, int indent)
{
    indented(level, "return_statement");
    if (ret->expression)
        print_expression(ret->expression, level + indent, indent);
    else
        indented(level + indent, "none_type");
}

static void print_block(const jac_ast_block block, int level, int indent)
{
    indented(level, "block");

    darray_foreach(block, jac_ast_scope_statement, statement)
    {
        switch (statement->holds)
        {
        case JAC_AST_STATEMENT_RETURN:
            print_statement_return(statement->opt.ret, level + indent, indent);
            break;
        case JAC_AST_BLOCK:
            print_block(statement->opt.block, level + indent, indent);
            break;
        case JAC_AST_EXPRESSION_STATEMENT:
            print_statement_expression(statement->opt.expr_statement, level + indent, indent);
            break;

        default:
            JAC_ASSERT(false, "unreachable.");
        }
    }
}

static void print_type(const jac_type *type, int level, int indent)
{
    if (type->indirection > 0)
    {
        indented_v(level, "pointer (%lu)", type->indirection);
        level += indent;
    }

    switch (type->kind)
    {
    case JAC_TYPE_NONE:
        indented(level, "none");
        break;
    case JAC_TYPE_CUSTOM:
        print_token(type->token, "custom", level);
        break;

    case JAC_TYPE_INT8:
    case JAC_TYPE_UINT8:
    case JAC_TYPE_INT32:
    case JAC_TYPE_UINT32:
    case JAC_TYPE_INT64:
    case JAC_TYPE_UINT64:
        print_token(type->token, "intrinsic", level);
        break;

    default:
        JAC_ASSERT(false, "unreachable.");
    }
}

static void print_variable_declaration(const jac_symbol *declaration, int level, int indent)
{
    indented(level, "variable_declaration");
    print_token(declaration->identifier, "id", level + indent);
    print_type(&declaration->type, level + indent, indent);
}

static void print_function_header(const jac_function *header, int level, int indent)
{
    indented(level, "function_header");
    print_token(header->identifier, "id", level + indent);
    indented_v(level + indent, "mangled: %s", header->mangled);
    print_type(&header->ret_type, level + indent, indent);

    darray_foreach(header->args, jac_symbol, arg) print_variable_declaration(arg, level + indent, indent);
}

static void print_extern_block(const jac_ast_extern_block block, int level, int indent)
{
    indented(level, "extern_block");
    darray_foreach(block, jac_function, header) print_function_header(header, level + indent, indent);
}

static void print_function_definition(const jac_ast_function_definition *func_def, int level, int indent)
{
    indented(level, "function_definition");
    print_function_header(&func_def->header, level + indent, indent);
    print_block(func_def->block, level + indent, indent);
}

void jac_print_unit(const jac_ast_unit unit, int indent)
{
    printf("unit\n");

    darray_foreach(unit, jac_ast_unit_statement, statement)
    {
        switch (statement->holds)
        {
        case JAC_AST_FUNCTION_DEFINITION:
            print_function_definition(statement->opt.func_def, indent, indent);
            break;
        case JAC_AST_EXTERN_BLOCK:
            print_extern_block(statement->opt.extern_block, indent, indent);
            break;

        default:
            JAC_ASSERT(false, "unreachable.");
        }
    }
}
