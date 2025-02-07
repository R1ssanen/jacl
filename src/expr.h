#ifndef _EXPR_H_
#define _EXPR_H_

#include "nodes.h"
#include "parse.h"

b8  jIsConstExpr(const jNodeExpression* expr);

f32 jSolveExpr(const jNodeExpression* expr, const jParser* parser);

f32 jSolveBinExpr(const jNodeExprBin* bin_expr, const jParser* parser);

#endif
