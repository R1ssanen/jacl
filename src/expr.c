#include "expr.h"

b8 jIsConstExpr(const jNodeExpr* expr) {
    switch (expr->has) {
    case JN_EXPR_ID: return !expr->id->mutable;
    case JN_BIN_EXPR: return jIsConstBinExpr(expr->bin_expr);
    case JN_LIT: return true;

    default: return false;
    }
}

b8 jIsConstBinExpr(const jNodeBinExpr* bin_expr) {
    if (!jIsConstExpr(bin_expr->lhs)) { return false; }
    if (!jIsConstExpr(bin_expr->rhs)) { return false; }
    return true;
}

f32 jSolveExpr(const jNodeExpr* expr) {

    switch (expr->has) {
    case JN_BIN_EXPR: return jSolveBinExpr(expr->bin_expr);
    case JN_LIT: {
        if (expr->lit->has == JN_LIT_STR) { return -1E9; }
        if (expr->lit->has == JN_LIT_INT) { return (f32)expr->lit->int_value; }
        return expr->lit->float_value;
    }
    case JN_EXPR_ID: return 0; // to be implemented

    default: return -1E9;
    }
}

f32 jSolveBinExpr(const jNodeBinExpr* bin_expr) {
    f32 l = jSolveExpr(bin_expr->lhs);
    f32 r = jSolveExpr(bin_expr->rhs);

    switch (bin_expr->op) {
    case JT_OP_ADD: return l + r;
    case JT_OP_SUB: return l - r;
    case JT_OP_MUL: return l * r;
    case JT_OP_DIV: return l / r;
    default: return -1E9;
    }
}
