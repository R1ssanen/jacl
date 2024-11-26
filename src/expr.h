#ifndef _EXPR_H_
#define _EXPR_H_

#include "nodes.h"

b8  jIsConstExpr(const jNodeExpr* expr);

b8  jIsConstBinExpr(const jNodeBinExpr* bin_expr);

f32 jSolveExpr(const jNodeExpr* expr);

f32 jSolveBinExpr(const jNodeBinExpr* bin_expr);

#endif
