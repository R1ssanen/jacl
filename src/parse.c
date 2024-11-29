#include "parse.h"

#include <stdio.h>
#include <stdlib.h>

#include "expr.h"
#include "hash.h"
#include "types.h"

jToken* jPeek(jParser* parser, u8 ahead) {

    if (parser->token_count <= 0) { return NULL; }
    return parser->tokens + ahead;
}

jToken* jConsume(jParser* parser) {

    if (parser->token_count <= 0) { return NULL; }

    parser->consumed++;
    parser->token_count--;
    return parser->tokens++;
}

void jUndo(jParser* parser, u8 count) {

    parser->consumed--;
    parser->token_count += count;
    parser->tokens -= count;
}

//  NOTE: MATCH EVERY CALL with jUndo to back from trying
jToken* jRequire(enum j_token_t type, jParser* parser, b8 require_category) {

    jToken* token = jConsume(parser);
    if (!token) { return NULL; }

    if ((token->type == type) || (require_category && (token->type & type))) { return token; }

    jUndo(parser, 1);
    return NULL;
}

b8 TryParseLit(jParser* parser, jNodeLit* lit) {

    jToken* token = jRequire(JT_LIT, parser, true);
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

    if (!jRequire(JT_SPEC_FORMAT, parser, false)) { return false; }

    jToken* token = jRequire(JT_LIT_STR, parser, false);
    if (!token) {
        jUndo(parser, 1);
        return false;
    }

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

        u64            hash = FNV_1A(id);
        jNodeStmtInit* var  = NULL;

        for (u64 j = 0; j < parser->var_count; ++j) {
            if (parser->vars[j].id->hash == hash) {
                var = &parser->vars[j];
                break;
            }
        }

        if (!var) {
            fprintf(stderr, "Format identifier '%s' doesn't exist.\n", id);
            return false;
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
            f32 val        = jSolveExpr(var->expr);

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
}

b8 TryParseExprId(jParser* parser, jNodeExprId* id) {

    jToken* token = jRequire(JT_ID, parser, true);
    if (!token) { return false; }

    id->id      = token->str;
    id->hash    = FNV_1A(token->str);
    id->mutable = false;
    return true;
}

void TryParseBinExpr(jParser* parser, jNodeExpr* expr, u8 min_prec) {

    // TryParseExpr(parser, expr, false, 0);

    jToken* token = jRequire(JT_OP, parser, true);
    if (!token) { return; }

    jNodeExpr lhs       = *expr;
    expr->bin_expr      = malloc(sizeof(jNodeBinExpr));
    expr->bin_expr->lhs = malloc(sizeof(jNodeExpr));
    expr->bin_expr->rhs = malloc(sizeof(jNodeExpr));
    expr->has           = JN_BIN_EXPR;

    while (token && (token->opinfo.prec >= min_prec)) {

        u8 next_min_prec = token->opinfo.prec;
        if (token->opinfo.assoc == J_ASSOC_LEFT) { next_min_prec += 1; }

        jNodeExpr rhs;
        // TryParseExpr(parser, &rhs, true, next_min_prec);
        TryParseBinExpr(parser, &rhs, next_min_prec);

        *expr->bin_expr->lhs = lhs;
        *expr->bin_expr->rhs = rhs;
        expr->bin_expr->op   = token->type;

        lhs                  = *expr;
        token                = jRequire(JT_OP, parser, true);
    }
}

b8 TryParseExpr(jParser* parser, jNodeExpr* expr) {

    do {
        jNodeLit lit;
        if (TryParseLit(parser, &lit)) {
            expr->lit  = malloc(sizeof(jNodeLit));
            *expr->lit = lit;
            expr->has  = JN_LIT;
            break;
        }

        jNodeExprId id;
        if (TryParseExprId(parser, &id)) {
            expr->id  = malloc(sizeof(jNodeExprId));
            *expr->id = id;
            expr->has = JN_EXPR_ID;
            break;
        }

        jNodeExprFStr fstr;
        if (TryParseExprFStr(parser, &fstr)) {
            expr->fstr  = malloc(sizeof(jNodeExprFStr));
            *expr->fstr = fstr;
            expr->has   = JN_EXPR_FSTR;
            break;
        }

        return false;
    } while (false);

    // if (parse_bin_expr) { TryParseBinExpr(parser, expr, min_prec); }

    return true;
}

b8 TryParseInit(jParser* parser, jNodeStmtInit* init) {

    jToken* type = jRequire(JT_TYPE, parser, true);
    if (!type) { return false; }

    if (!jRequire(JT_SPEC_COLON, parser, false)) {
        jUndo(parser, 1);
        return false;
    }

    jNodeExprId id;
    if (!TryParseExprId(parser, &id)) {
        jUndo(parser, 2);
        return false;
    }

    if (!jRequire(JT_SPEC_COLON, parser, false)) {
        jUndo(parser, 3);
        return false;
    }

    if (!jRequire(JT_OP_EQ, parser, false)) {
        jUndo(parser, 4);
        return false;
    }

    jNodeExpr expr;
    if (!TryParseExpr(parser, &expr)) {
        jUndo(parser, 4);
        return false;
    }

    init->type                        = type->type;
    init->id                          = malloc(sizeof(jNodeExprId));
    *init->id                         = id;
    init->expr                        = malloc(sizeof(jNodeExpr));
    *init->expr                       = expr;

    parser->vars[parser->var_count++] = *init;

    return true;
}

b8 TryParseExit(jParser* parser, jNodeStmtExit* exit) {

    if (!jRequire(JT_KEY_EXIT, parser, false)) { return false; }

    jNodeExpr expr;
    if (!TryParseExpr(parser, &expr)) {
        jUndo(parser, 1);
        return false;
    }

    if (expr.has == JN_BIN_EXPR) {
        f32 ans = jSolveBinExpr(expr.bin_expr);
        fprintf(stderr, "ans = %f\n", ans);
    }

    exit->expr  = malloc(sizeof(jNodeExpr));
    *exit->expr = expr;
    return true;
}

enum j_error_t TryParseStmt(jParser* parser, jNodeStmt* stmt) {

    do {
        jNodeStmtExit exit;
        if (TryParseExit(parser, &exit)) {
            stmt->exit  = malloc(sizeof(jNodeStmtExit));
            *stmt->exit = exit;
            stmt->has   = JN_STMT_EXIT;
            break;
        }

        jNodeStmtInit init;
        if (TryParseInit(parser, &init)) {
            stmt->init  = malloc(sizeof(jNodeStmtInit));
            *stmt->init = init;
            stmt->has   = JN_STMT_INIT;
            break;
        }

        return J_ERROR;
    } while (false);

    if (!jRequire(JT_SPEC_SEMI, parser, false)) { return J_ERROR; }
    return J_SUCCESS;
}

enum j_error_t jParse(jParser* parser, jNodeRoot* program) {

    enum j_error_t err;

    while (parser->token_count > 0) {
        jNodeStmt stmt;
        if (!(err = TryParseStmt(parser, &stmt))) {
            program->stmts[program->stmt_count++] = stmt;
            continue;
        }

        fputs("\nTokenS LEFT ON parser:\n", stderr);
        for (u64 i = 0; i < parser->token_count; ++i) { jPrintDebugToken(&parser->tokens[i]); }
        fputs("\n", stderr);
        return err;
    }

    return J_SUCCESS;
}
