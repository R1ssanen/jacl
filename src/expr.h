#ifndef _EXPR_H_
#define _EXPR_H_

#include "nodes.h"
#include "parse.h"

b8  jIsConstExpr(const jNodeExpr* expr);

f32 jSolveExpr(const jNodeExpr* expr, const jParser* parser);

f32 jSolveBinExpr(const jNodeBinExpr* bin_expr, const jParser* parser);

#endif
