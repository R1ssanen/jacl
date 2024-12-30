#include "expr.h"

#include "parse.h"

b8 jIsConstBinExpr(const jNodeBinExpr* bin_expr) {
    if (!jIsConstExpr(bin_expr->lhs)) { return false; }
    if (!jIsConstExpr(bin_expr->rhs)) { return false; }
    return true;
}

b8 jIsConstExpr(const jNodeExpr* expr) {
    switch (expr->has) {
    case JN_EXPR_ID: return !expr->id->is_mutable;
    case JN_BIN_EXPR: return jIsConstBinExpr(expr->bin_expr);
    case JN_LIT: return true;

    default: return false;
    }
}

f32 jSolveExpr(const jNodeExpr* expr, const jParser* parser) {
    switch (expr->has) {
    case JN_BIN_EXPR: return jSolveBinExpr(expr->bin_expr, parser);
    case JN_LIT: {
        if (expr->lit->has == JN_LIT_STR) { return -1E9; }
        if (expr->lit->has == JN_LIT_INT) { return (f32)expr->lit->int_value; }
        return expr->lit->float_value;
    }
    case JN_EXPR_ID: {
        jNodeExpr* id_expr = _jGetVarExpr(expr->id, parser);
        if (id_expr) { return jSolveExpr(id_expr, parser); }
    }

    default: return -1E9;
    }
}

f32 jSolveBinExpr(const jNodeBinExpr* bin_expr, const jParser* parser) {
    f32 l = jSolveExpr(bin_expr->lhs, parser);
    f32 r = jSolveExpr(bin_expr->rhs, parser);

    switch (bin_expr->op) {
    case JT_OP_ADD: return l + r;
    case JT_OP_SUB: return l - r;
    case JT_OP_MUL: return l * r;
    case JT_OP_DIV: return l / r;

    default: return -1E9;
    }
}
