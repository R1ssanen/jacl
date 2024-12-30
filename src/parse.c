#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "expr.h"
#include "hash.h"
#include "mem.h"
#include "types.h"

void jDumpTokens(const jParser* parser) {
    fputs("TOKEN STACK DUMP:\n", stderr);

    for (u64 i = parser->curr_ptr; i < J_ARRAY_SIZE(parser->tokens); ++i) {
        jPrintDebugToken(&parser->tokens[i]);
    }

    fputs("\n", stderr);
}

b8 jParserIsEOF(const jParser* parser) { return parser->curr_ptr >= J_ARRAY_SIZE(parser->tokens); }

jToken* jParserPeek(const jParser* parser, u8 ahead) {
    if (jParserIsEOF(parser)) { return NULL; }
    return parser->tokens + parser->curr_ptr;
}

jToken* jParserConsume(jParser* parser) {
    jToken* token = jParserPeek(parser, 0);
    if (!token) { return NULL; }

    parser->curr_ptr++;
    return token;
}

// push the token stack back to point
void jParserRevert(jParser* parser, u64 point) {
    if (parser->curr_ptr < point) { return; }
    u64 offset = parser->curr_ptr - point;
    parser->curr_ptr -= offset;
}

jToken* jParserRequire(enum j_token_t type, jParser* parser, b8 require_category) {
    jToken* token = jParserConsume(parser);
    if (!token) { return NULL; }

    if (require_category && (token->type & type)) { return token; }
    if (token->type == type) { return token; }

    parser->curr_ptr--;
    return NULL;
}

u64 jParserGetLine(const jParser* parser) { return jParserPeek(parser, 0)->line; }

u64 jParserGetColumn(const jParser* parser) { return jParserPeek(parser, 0)->col; }

b8  TryParseLit(jParser* parser, jNodeLit* lit) {
    jToken* token = jParserRequire(JT_LIT, parser, true);
    if (!token) { return false; }

    switch (token->type) {
    case JT_LIT_INT: {
        lit->int_value = token->int_value;
        lit->has       = JN_LIT_INT;
        return true;
    }
    case JT_LIT_FLOAT: {
        lit->float_value = token->float_value;
        lit->has         = JN_LIT_FLOAT;
        return true;
    }
    case JT_LIT_STR: {
        lit->str_value = token->str;
        lit->has       = JN_LIT_STR;
        return true;
    }

    default: return false;
    }
}

b8 TryParseExprFStr(jParser* parser, jNodeExprFStr* fstr) {
    u64 state = parser->curr_ptr;

    if (!jParserRequire(JT_SPEC_FORMAT, parser, false)) { goto fail; }

    jToken* token = jParserRequire(JT_LIT_STR, parser, false);
    if (!token) { goto fail; }

    char fmt[2048];
    u64  fmt_len         = 0;
    fstr->id_count       = 0;

    const char* str      = token->str;
    u64         prev_end = 0;

    for (u64 i = 0; i < strlen(str); ++i) {
        if (str[i] != '{') { continue; }

        u64 new_start = ++i;
        while (str[i] != '}') { ++i; }

        u64  len = i - new_start;
        char id[len + 1];
        strncpy(id, str + new_start, len);
        id[len]             = '\0';

        u64            hash = FNV_1A(id, strlen(id));
        jNodeStmtInit* var  = NULL;

        for (u64 j = 0; j < parser->var_count; ++j) {
            if (parser->vars[j].id->hash == hash) {
                var = &parser->vars[j];
                break;
            }
        }

        if (!var) {
            fprintf(stderr, "Format identifier '%s' doesn't exist.\n", id);
            goto fail;
        }

        u64 new_len = new_start - prev_end - 1;
        strncpy(fmt + fmt_len, str + prev_end, new_len);

        fstr->ids[fstr->id_count++] = hash;
        prev_end                    = new_start + len + 1;
        fmt_len += new_len;

        if (!jIsConstExpr(var->expr)) {
            fmt[fmt_len++] = '%';
            fmt[fmt_len++] = 's';
        } else {
            f32 val        = jSolveExpr(var->expr, parser);

            fmt[fmt_len++] = '%';
            fmt[fmt_len++] = 'f';
            sprintf(fmt, fmt, val);
        }
    }

    fmt[fmt_len++] = '\0';

    char* _fmt     = malloc(fmt_len * sizeof(char));
    strncpy(_fmt, fmt, fmt_len);
    fstr->fmt = _fmt;

    fprintf(stderr, "%s", fstr->fmt);
    return true;

fail:
    jParserRevert(parser, state);
    return false;
}

b8 TryParseIdent(jParser* parser, jNodeExprId* id) {
    u64     state = parser->curr_ptr;

    jToken* token = jParserRequire(JT_ID, parser, true);
    if (!token) { goto fail; }

    id->id         = token->str;
    id->hash       = FNV_1A(token->str, strlen(token->str));
    id->is_mutable = false;
    return true;

fail:
    jParserRevert(parser, state);
    return false;
}

b8 TryParseExpr(jParser*, jNodeExpr*, jErrorHandler*, u8);

b8 TryParseAtom(jParser* parser, jNodeExpr* atom, jErrorHandler* err) {
    u64 state = parser->curr_ptr;

    // subexpression
    if (jParserRequire(JT_SPEC_LPAREN, parser, false)) {
        if (!TryParseExpr(parser, atom, err, 0)) { goto fail; }
        if (!jParserRequire(JT_SPEC_RPAREN, parser, false)) { goto fail; }

        return true;
    }

    jNodeLit lit;
    if (TryParseLit(parser, &lit)) {
        atom->lit  = malloc(sizeof(jNodeLit));
        *atom->lit = lit;
        atom->has  = JN_LIT;

        return true;
    }

    jNodeExprId id;
    if (TryParseIdent(parser, &id)) {
        atom->id  = malloc(sizeof(jNodeExprId));
        *atom->id = id;
        atom->has = JN_EXPR_ID;

        return true;
    }

    jNodeExprFStr fstr;
    if (TryParseExprFStr(parser, &fstr)) {
        atom->fstr  = malloc(sizeof(jNodeExprFStr));
        *atom->fstr = fstr;
        atom->has   = JN_EXPR_FSTR;

        return true;
    }

fail:
    jParserRevert(parser, state);
    return false;
}

b8 TryParseExpr(jParser* parser, jNodeExpr* expr, jErrorHandler* err, u8 min_bind) {
    u64 state = parser->curr_ptr;
    if (!TryParseAtom(parser, expr, err)) { goto fail; }

    jToken* op = jParserPeek(parser, 0);
    if (!(op->type & JT_OP) || (op->opinfo.lbp < min_bind)) { return true; }

    jNodeExpr lhs = *expr;
    expr->has     = JN_BIN_EXPR;

    do {
        jParserConsume(parser);
        jNodeExpr rhs;
        if (!TryParseExpr(parser, &rhs, err, op->opinfo.rbp)) { goto fail; }

        expr->bin_expr       = malloc(sizeof(jNodeBinExpr));
        expr->bin_expr->lhs  = malloc(sizeof(jNodeExpr));
        expr->bin_expr->rhs  = malloc(sizeof(jNodeExpr));

        *expr->bin_expr->lhs = lhs;
        *expr->bin_expr->rhs = rhs;
        expr->bin_expr->op   = op->type;

        lhs                  = *expr;
        op                   = jParserPeek(parser, 0);
    } while ((op->type & JT_OP) && (op->opinfo.lbp >= min_bind));

    return true;

fail:
    jRaiseError(
        err, JE_SYNTAX, "Invalid expression.", jParserGetLine(parser), jParserGetColumn(parser)
    );

    jParserRevert(parser, state);
    return false;
}

b8 TryParseInit(jParser* parser, jNodeStmtInit* init, jErrorHandler* err) {
    u64     state = parser->curr_ptr;

    jToken* type  = jParserRequire(JT_TYPE, parser, true);
    if (!type) { goto fail; }

    if (!jParserRequire(JT_SPEC_COLON, parser, false)) { goto fail; }

    jNodeExprId id;
    if (!TryParseIdent(parser, &id)) { goto fail; }

    if (!jParserRequire(JT_SPEC_COLON, parser, false)) { goto fail; }

    if (!jParserRequire(JT_OP_EQ, parser, false)) { goto fail; }

    jNodeExpr expr;
    if (!TryParseExpr(parser, &expr, err, 0)) { goto fail; }

    init->type                        = type->type;
    init->id                          = malloc(sizeof(jNodeExprId));
    *init->id                         = id;
    init->expr                        = malloc(sizeof(jNodeExpr));
    *init->expr                       = expr;

    parser->vars[parser->var_count++] = *init;
    return true;

fail:
    jParserRevert(parser, state);
    return false;
}

b8 TryParseAssign(jParser* parser, jNodeStmtAssign* assign, jErrorHandler* err) {
    u64         state = parser->curr_ptr;

    jNodeExprId id;
    if (!TryParseIdent(parser, &id)) { goto fail; }

    if (!jParserRequire(JT_SPEC_COLON, parser, false)) { goto fail; }
    if (!jParserRequire(JT_OP_EQ, parser, false)) { goto fail; }

    jNodeExpr expr;
    if (!TryParseExpr(parser, &expr, err, 0)) { goto fail; }

    assign->id    = malloc(sizeof(jNodeExprId));
    *assign->id   = id;
    assign->expr  = malloc(sizeof(jNodeExpr));
    *assign->expr = expr;

    return true;

fail:
    jParserRevert(parser, state);
    return false;
}

b8 TryParseExit(jParser* parser, jNodeStmtExit* exit, jErrorHandler* err) {
    u64 state = parser->curr_ptr;

    if (!jParserRequire(JT_KEY_EXIT, parser, false)) { goto fail; }

    jNodeExpr expr;
    if (!TryParseExpr(parser, &expr, err, 0)) { goto fail; }

    f32 solution = jSolveExpr(&expr, parser);
    fprintf(stderr, "expr = %f\n", solution);

    exit->expr  = malloc(sizeof(jNodeExpr));
    *exit->expr = expr;
    return true;

fail:
    jParserRevert(parser, state);
    return false;
}

b8 TryParseStmt(jParser* parser, jNodeStmt* stmt, jErrorHandler* err) {
    jNodeStmtExit exit;
    if (TryParseExit(parser, &exit, err)) {
        stmt->exit  = malloc(sizeof(jNodeStmtExit));
        *stmt->exit = exit;
        stmt->has   = JN_STMT_EXIT;

        goto success;
    }

    jNodeStmtInit init;
    if (TryParseInit(parser, &init, err)) {
        stmt->init  = malloc(sizeof(jNodeStmtInit));
        *stmt->init = init;
        stmt->has   = JN_STMT_INIT;

        /*if (jIsConstExpr(stmt->init->expr)) {
            printf("init_val: %f\n", jSolveExpr(stmt->init->expr, parser));
        }*/

        goto success;
    }

    jRaiseError(
        err, JE_SYNTAX, "Invalid statement.", jParserGetLine(parser), jParserGetColumn(parser)
    );

    return false;

success:
    if (jParserRequire(JT_SPEC_SEMI, parser, false)) { return true; }

    jRaiseError(
        err, JE_SYNTAX, "Missing semicolon.", jParserGetLine(parser), jParserGetColumn(parser)
    );
    return false;
}

b8 jParse(jParser* parser, jNodeStmt* stmts, jErrorHandler* err) {

    while (!jParserIsEOF(parser)) {
        jNodeStmt stmt;
        if (!TryParseStmt(parser, &stmt, err)) {
            if (err->error_count >= J_MAX_ERRORS) { return false; }

            // skip to next statement
            while (jParserConsume(parser)->type != JT_SPEC_SEMI);
        } else {
            J_ARRAY_PUSH(stmts, stmt);
        }
    }

    // errors thrown
    if (err->error_count) { return false; }
    return true;
}
