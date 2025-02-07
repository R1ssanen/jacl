#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "hash.h"
#include "tokens.h"
#include "types.h"

static b8      IsEOF(const jParser* parser) { return parser->tokens[parser->ptr].type == JT_EOF; }

static jToken* Peek(const jParser* parser) { return parser->tokens + parser->ptr; }

static jToken* Consume(jParser* parser) {
    jToken* token = Peek(parser);
    parser->ptr++;
    return token;
}

// push the token stack back to point
static void Revert(jParser* parser, u64 point) {
    if (parser->ptr < point) { return; }
    u64 offset = parser->ptr - point;
    parser->ptr -= offset;
}

static jToken* Require(enum j_token_t type, jParser* parser, b8 require_category) {
    jToken* token = Consume(parser);
    if (require_category && (token->type & type)) { return token; }
    if (token->type == type) { return token; }

    parser->ptr--;
    return NULL;
}

b8 jParseLiteral(jParser* parser, jNodeLiteral* lit, jError** errors) {
    jToken* token = Require(JT_LIT, parser, true);
    if (!token) { return false; }

    lit->value = token->data;

    switch (token->type) {
    case JT_LIT_INT:
        lit->int_value = strtoll((const char*)token->data, NULL, 10);
        lit->has       = JN_LIT_INT;
        return true;

    case JT_LIT_FLOAT:
        lit->float_value = strtod((const char*)token->data, NULL);
        lit->has         = JN_LIT_FLOAT;
        return true;

    case JT_LIT_STR: lit->has = JN_LIT_STR; return true;

    default: return false;
    }
}

b8 jParseIdentifier(jParser* parser, jNodeExprIdent* ident, jError** errors) {
    u64     state = parser->ptr;

    jToken* token = Require(JT_ID, parser, true);
    if (!token) { goto fail; }

    ident->hash          = FNV_1A(token->data, strlen((const char*)token->data));
    jNodeStatInit* found = _jFindVariable(ident, parser);
    if (found) {
        ident = found->id;
    } else {
        ident->id         = token->data;
        ident->is_mutable = false;
    }

    return true;

fail:
    Revert(parser, state);
    return false;
}

b8 jParseExpression(jParser*, jNodeExpression*, jError**, u8);

b8 jParseExpressionAtom(jParser* parser, jNodeExpression* atom, jError** errors) {
    u64 state = parser->ptr;

    // subexpression
    if (Require(JT_SPEC_LPAREN, parser, false)) {
        if (!jParseExpression(parser, atom, errors, 0)) {
            J_ARRAY_PUSH(
                *errors, J_NEW_ERROR(JE_SYNTAX, "Invalid parenthesised expression.", Peek(parser))
            );
            goto fail;
        }
        if (!Require(JT_SPEC_RPAREN, parser, false)) {
            J_ARRAY_PUSH(*errors, J_NEW_ERROR(JE_SYNTAX, "Unclosed parentheses.", Peek(parser)));
            goto fail;
        }
        return true;
    }

    jNodeLiteral lit;
    if (jParseLiteral(parser, &lit, errors)) {
        atom->lit  = malloc(sizeof(jNodeLiteral));
        *atom->lit = lit;
        atom->has  = JN_LITERAL;
        return true;
    }

    jNodeExprIdent ident;
    if (jParseIdentifier(parser, &ident, errors)) {
        atom->id  = malloc(sizeof(jNodeExprIdent));
        *atom->id = ident;
        atom->has = JN_EXPR_ID;
        return true;
    }

fail:
    Revert(parser, state);
    return false;
}

b8 jParseExpression(jParser* parser, jNodeExpression* expr, jError** errors, u8 min_bind) {
    u64 state = parser->ptr;
    if (!jParseExpressionAtom(parser, expr, errors)) { goto fail; }

    jToken* op = Peek(parser);
    if (!(op->type & JT_OP) || (op->opinfo.lbp < min_bind)) { return true; }

    jNodeExpression lhs = *expr;
    expr->has           = JN_EXPR_BIN;
    expr->bin_expr      = malloc(sizeof(jNodeExprBin));
    expr->bin_expr->lhs = malloc(sizeof(jNodeExpression));

    do {
        Consume(parser);
        jNodeExpression rhs;
        if (!jParseExpression(parser, &rhs, errors, op->opinfo.rbp)) { goto fail; }

        expr->bin_expr->rhs  = malloc(sizeof(jNodeExpression));

        *expr->bin_expr->lhs = lhs;
        *expr->bin_expr->rhs = rhs;
        expr->bin_expr->op   = op->type;

        lhs                  = *expr;
        op                   = Peek(parser);
    } while ((op->type & JT_OP) && (op->opinfo.lbp >= min_bind));

    return true;

fail:
    Revert(parser, state);
    return false;
}

#include "expr.h"

b8 jParseStatementInit(jParser* parser, jNodeStatInit* init, jError** errors) {
    u64     old  = parser->ptr;

    jToken* type = Require(JT_TYPE, parser, true);
    if (!type) { goto fail; }

    if (!Require(JT_SPEC_COLON, parser, false)) { goto fail; }

    b8 is_mutable = false;
    if (Require(JT_KEY_MUT, parser, false)) { is_mutable = true; }

    jNodeExprIdent ident;
    if (!jParseIdentifier(parser, &ident, errors)) { goto fail; }

    if (!Require(JT_SPEC_COLON, parser, false)) { goto fail; }

    if (!Require(JT_OP_EQ, parser, false)) { goto fail; }

    jNodeExpression expr;
    if (!jParseExpression(parser, &expr, errors, 0)) {
        J_ARRAY_PUSH(
            *errors, J_NEW_ERROR(
                         JE_SYNTAX, "Variable assignment must be followed by a valid expression.",
                         Peek(parser)
                     )
        );
        goto fail;
    }
    // fprintf(stderr, "expr = %f\n", jSolveExpr(&expr, parser));

    init->type           = type->type;
    init->id             = malloc(sizeof(jNodeExprIdent));
    *init->id            = ident;
    init->id->is_mutable = is_mutable;
    init->expr           = malloc(sizeof(jNodeExpression));
    *init->expr          = expr;

    J_ARRAY_PUSH(parser->vars, *init);
    return true;

fail:
    Revert(parser, old);
    return false;
}

b8 jParseStatementAssign(jParser* parser, jNodeStatAssign* assign, jError** errors) {
    u64            old = parser->ptr;

    jNodeExprIdent ident;
    if (!jParseIdentifier(parser, &ident, errors)) { goto fail; }

    if (!Require(JT_SPEC_COLON, parser, false)) { goto fail; }

    if (!Require(JT_OP_EQ, parser, false)) { goto fail; }

    jNodeExpression expr;
    if (!jParseExpression(parser, &expr, errors, 0)) {
        J_ARRAY_PUSH(
            *errors,
            J_NEW_ERROR(JE_SYNTAX, "Assignment must be followed by an expression.", Peek(parser))
        );
        goto fail;
    }

    assign->id    = malloc(sizeof(jNodeExprIdent));
    *assign->id   = ident;
    assign->expr  = malloc(sizeof(jNodeExpression));
    *assign->expr = expr;

    return true;

fail:
    Revert(parser, old);
    return false;
}

b8 jParseStatementExit(jParser* parser, jNodeStatExit* exit, jError** errors) {
    u64 old = parser->ptr;

    if (!Require(JT_KEY_EXIT, parser, false)) { goto fail; }

    jNodeExpression expr;
    if (!jParseExpression(parser, &expr, errors, 0)) {
        J_ARRAY_PUSH(*errors, J_NEW_ERROR(JE_SYNTAX, "Invalid 'exit' expression.", Peek(parser)));
        goto fail;
    }

    exit->expr  = malloc(sizeof(jNodeExpression));
    *exit->expr = expr;
    return true;

fail:
    Revert(parser, old);
    return false;
}

b8 jParseStatement(jParser* parser, jNodeStatement* statement, jError** errors) {
    do {
        jNodeStatExit exit;
        if (jParseStatementExit(parser, &exit, errors)) {
            statement->exit  = malloc(sizeof(jNodeStatExit));
            *statement->exit = exit;
            statement->has   = JN_STAT_EXIT;
            break;
        }

        jNodeStatInit init;
        if (jParseStatementInit(parser, &init, errors)) {
            statement->init  = malloc(sizeof(jNodeStatInit));
            *statement->init = init;
            statement->has   = JN_STAT_INIT;
            break;
        }

        J_ARRAY_PUSH(*errors, J_NEW_ERROR(JE_SYNTAX, "Invalid statement.", Peek(parser)));
        return false;

    } while (false);

    if (!Require(JT_SPEC_SEMI, parser, false)) {
        J_ARRAY_PUSH(*errors, J_NEW_ERROR(JE_SYNTAX, "Missing semicolon.", Peek(parser)));
        return false;
    }

    return true;
}

b8 jParse(jParser* parser, jNodeStatement** statements, jError** errors) {
    jNodeStatement statement;

    while (!IsEOF(parser)) {
        if (jParseStatement(parser, &statement, errors)) {
            J_ARRAY_PUSH(*statements, statement);
        } else {
            if (J_ARRAY_SIZE(*errors) >= J_MAX_ERRORS) { return false; }
            while (!IsEOF(parser) && Consume(parser)->type != JT_SPEC_SEMI);
        }
    }

    // errors thrown
    if (J_ARRAY_SIZE(*errors)) { return false; }
    return true;
}
