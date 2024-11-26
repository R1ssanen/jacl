#include "parse.h"

#include <stdio.h>
#include <stdlib.h>

#include "expr.h"
#include "hash.h"
#include "types.h"

typedef struct {
    jToken* tokens;
    u64     token_count;
    u64     consumed;

    //  NOTE: TEMPORARY!
    jNodeStmtInit vars[256];
    u64           var_count;
} jTokenStack;

static jToken* jPeek(jTokenStack* stack, u8 ahead) {
    if (stack->token_count <= 0) { return NULL; }
    return stack->tokens + ahead;
}

static jToken* jConsume(jTokenStack* stack) {
    if (stack->token_count <= 0) { return NULL; }

    stack->consumed++;
    stack->token_count--;
    return stack->tokens++;
}

static void jUndo(jTokenStack* stack, u8 count) {
    stack->consumed--;
    stack->token_count += count;
    stack->tokens -= count;
}

//  NOTE: MATCH EVERY CALL with jUndo to back from trying
static jToken* jRequire(enum j_token_t type, jTokenStack* stack, b8 require_category) {
    jToken* token = jConsume(stack);
    if (!token) { return NULL; }

    if (require_category) {
        if (token->type & type) { return token; }
        jUndo(stack, 1);
        return NULL;
    }

    if (token->type == type) { return token; }
    jUndo(stack, 1);
    return NULL;
}

static b8 TryParseLit(jTokenStack* stack, jNodeLit* lit) {
    jToken* token = jRequire(JT_LIT, stack, true);
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

static b8 TryParseExprFStr(jTokenStack* stack, jNodeExprFStr* fstr) {
    if (!jRequire(JT_SPEC_FORMAT, stack, false)) { return false; }

    jToken* token = jRequire(JT_LIT_STR, stack, false);
    if (!token) {
        jUndo(stack, 1);
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

        for (u64 j = 0; j < stack->var_count; ++j) {
            if (stack->vars[j].id->hash == hash) {
                var = &stack->vars[j];
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

static b8 TryParseExprId(jTokenStack* stack, jNodeExprId* id) {
    jToken* token = jRequire(JT_ID, stack, true);
    if (!token) { return false; }

    id->id      = token->str;
    id->hash    = FNV_1A(token->str);
    id->mutable = false;
    return true;
}

/*
#if 0
static b8 TryParseExpr(jTokenStack* stack, jNodeExpr* expr, u8 min_prec) {
    if (!TryParseLit_(stack, expr)) {
        fputs("1\n", stderr);
        return false;
    }

    jToken* token = jRequire(JC_OP, stack);
    if (!token) { return true; }

    jNodeExpr lhs       = *expr;
    expr->bin_expr      = malloc(sizeof(jNodeBinExpr));
    expr->bin_expr->lhs = malloc(sizeof(jNodeExpr));
    expr->bin_expr->rhs = malloc(sizeof(jNodeExpr));
    expr->has           = JN_BIN_EXPR;

    while (token && (token->opinfo.prec >= min_prec)) {

        u8 next_min_prec = token->opinfo.prec;
        if (token->opinfo.assoc == J_ASSOC_LEFT) { next_min_prec += 1; }

        fprintf(stderr, "min_prec = %i\n", next_min_prec);

        jNodeExpr rhs;
        if (!TryParseExpr(stack, &rhs, next_min_prec)) {
            fputs("2\n", stderr);
            return false;
        }

        *expr->bin_expr->lhs = lhs;
        *expr->bin_expr->rhs = rhs;
        expr->bin_expr->op   = token->type;

        lhs                  = *expr;
        token                = jRequire(JC_OP, stack);
    }

    return true;
}
#else
b8 TryParseExpr(jTokenStack* stack, jNodeExpr* expr, u8 min_prec) {
    if (!TryParseLit_(stack, expr)) { return false; }

    jToken* curr = jRequire(JC_OP, stack);
    if (!curr) { return true; }

    jNodeExpr lhs       = *expr;
    expr->bin_expr      = malloc(sizeof(jNodeBinExpr));
    expr->bin_expr->lhs = malloc(sizeof(jNodeExpr));
    expr->bin_expr->rhs = malloc(sizeof(jNodeExpr));
    expr->has           = JN_BIN_EXPR;

    while (curr && (curr->opinfo.prec >= min_prec)) {
        jToken*   prev = curr;

        jNodeExpr rhs;
        if (!TryParseLit_(stack, &rhs)) { return false; }

        curr = jRequire(JC_OP, stack);
        while (curr && ((curr->opinfo.prec > prev->opinfo.prec) ||
                        ((curr->opinfo.assoc == J_ASSOC_RIGHT) &&
                         (curr->opinfo.prec == prev->opinfo.prec)))) {

            u8 next_min_prec = prev->opinfo.prec;
            if (curr->opinfo.prec > prev->opinfo.prec) { next_min_prec += 1; }

            TryParseExpr(stack, &rhs, next_min_prec);
            curr = jRequire(JC_OP, stack);
        }

        *expr->bin_expr->lhs = lhs;
        *expr->bin_expr->rhs = rhs;
        expr->bin_expr->op   = prev->type;

        lhs                  = *expr;
    }

    return true;
}
#endif
*/

#if 1
static void TryParseBinExpr(jTokenStack* stack, jNodeExpr* expr, u8 min_prec) {

    // TryParseExpr(stack, expr, false, 0);

    jToken* token = jRequire(JT_OP, stack, true);
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
        // TryParseExpr(stack, &rhs, true, next_min_prec);
        TryParseBinExpr(stack, &rhs, next_min_prec);

        *expr->bin_expr->lhs = lhs;
        *expr->bin_expr->rhs = rhs;
        expr->bin_expr->op   = token->type;

        lhs                  = *expr;
        token                = jRequire(JT_OP, stack, true);
    }
}
#endif

b8 TryParseExpr(jTokenStack* stack, jNodeExpr* expr) {
    do {
        jNodeLit lit;
        if (TryParseLit(stack, &lit)) {
            expr->lit  = malloc(sizeof(jNodeLit));
            *expr->lit = lit;
            expr->has  = JN_LIT;
            break;
        }

        jNodeExprId id;
        if (TryParseExprId(stack, &id)) {
            expr->id  = malloc(sizeof(jNodeExprId));
            *expr->id = id;
            expr->has = JN_EXPR_ID;
            break;
        }

        jNodeExprFStr fstr;
        if (TryParseExprFStr(stack, &fstr)) {
            expr->fstr  = malloc(sizeof(jNodeExprFStr));
            *expr->fstr = fstr;
            expr->has   = JN_EXPR_FSTR;
            break;
        }

        return false;
    } while (false);

    // if (parse_bin_expr) { TryParseBinExpr(stack, expr, min_prec); }

    return true;
}

static b8 TryParseInit(jTokenStack* stack, jNodeStmtInit* init) {
    jToken* type = jRequire(JT_TYPE, stack, true);
    if (!type) { return false; }

    if (!jRequire(JT_SPEC_COLON, stack, false)) {
        jUndo(stack, 1);
        return false;
    }

    jNodeExprId id;
    if (!TryParseExprId(stack, &id)) {
        jUndo(stack, 2);
        return false;
    }

    if (!jRequire(JT_SPEC_COLON, stack, false)) {
        jUndo(stack, 3);
        return false;
    }

    if (!jRequire(JT_OP_EQ, stack, false)) {
        jUndo(stack, 4);
        return false;
    }

    jNodeExpr expr;
    if (!TryParseExpr(stack, &expr)) {
        jUndo(stack, 4);
        return false;
    }

    init->type                      = type->type;
    init->id                        = malloc(sizeof(jNodeExprId));
    *init->id                       = id;
    init->expr                      = malloc(sizeof(jNodeExpr));
    *init->expr                     = expr;

    stack->vars[stack->var_count++] = *init;

    return true;
}

static b8 TryParseExit(jTokenStack* stack, jNodeStmtExit* exit) {
    if (!jRequire(JT_KEY_EXIT, stack, false)) { return false; }

    jNodeExpr expr;
    if (!TryParseExpr(stack, &expr)) {
        jUndo(stack, 1);
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

static enum j_error_t TryParseStmt(jTokenStack* stack, jNodeStmt* stmt) {

    do {
        jNodeStmtExit exit;
        if (TryParseExit(stack, &exit)) {
            stmt->exit  = malloc(sizeof(jNodeStmtExit));
            *stmt->exit = exit;
            stmt->has   = JN_STMT_EXIT;
            break;
        }

        jNodeStmtInit init;
        if (TryParseInit(stack, &init)) {
            stmt->init  = malloc(sizeof(jNodeStmtInit));
            *stmt->init = init;
            stmt->has   = JN_STMT_INIT;
            break;
        }

        return J_ERROR;
    } while (false);

    if (!jRequire(JT_SPEC_SEMI, stack, false)) { return J_ERROR; }
    return J_SUCCESS;
}

enum j_error_t jParse(jToken* tokens, u64 token_count, jNodeRoot* program) {
    enum j_error_t err;
    jTokenStack    stack = { tokens, token_count };

    while (stack.token_count > 0) {
        jNodeStmt stmt;
        if (!(err = TryParseStmt(&stack, &stmt))) {
            program->stmts[program->stmt_count++] = stmt;
        } else {
            fputs("\nTOKENS LEFT ON STACK:\n", stderr);
            for (u64 i = 0; i < stack.token_count; ++i) { jPrintDebugToken(&stack.tokens[i]); }
            fputs("\n", stderr);
            return err;
        }
    }

    return J_SUCCESS;
}
