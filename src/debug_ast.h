#ifndef _DEBUG_AST_H_
#define _DEBUG_AST_H_

#include <stdio.h>

#include "array.h"
#include "nodes.h"
#include "tokens.h"

static inline void _jPrintOffset(i32 offset) { fprintf(stderr, "%*s", offset, ""); }

static inline void _jPrintType(enum j_token_t type, u64 level) {
    _jPrintOffset(level);
    fprintf(stderr, "type = %s\n", jGetTokenString(type));
}

static inline void _jPrintOp(enum j_token_t op, u64 level) {
    _jPrintOffset(level);
    fprintf(stderr, "op = (%s)\n", jGetTokenString(op));
}

static inline void _jPrintFloat(f64 num, u64 level) {
    _jPrintOffset(level);
    fprintf(stderr, "float = %f\n", num);
}

static inline void _jPrintInt(i64 num, u64 level) {
    _jPrintOffset(level);
    fprintf(stderr, "int = %lli\n", num);
}

static inline void _jPrintStr(const u8* str, u64 level) {
    _jPrintOffset(level);
    fprintf(stderr, "str = '%s'\n", str);
}

static inline void _jPrintLit(const jNodeLiteral* lit, u64 level) {
    _jPrintOffset(level);
    fputs("lit:\n", stderr);
    _jPrintOffset(level);
    fputs("   \\\n", stderr);

    switch (lit->has) {
    case JN_LIT_INT: _jPrintInt(lit->int_value, level + 2); return;
    case JN_LIT_FLOAT: _jPrintFloat(lit->float_value, level + 2); return;
    case JN_LIT_STR: _jPrintStr(lit->value, level + 2); return;

    default: return;
    }
}

static inline void _jPrintId(jNodeExprIdent* id, u64 level) {
    _jPrintOffset(level);
    fputs("id:\n", stderr);

    _jPrintOffset(level);
    fputs("   \\\n", stderr);
    _jPrintOffset(level + 2);
    fprintf(stderr, "ident = '%s'\n", id->id);

    /*_jPrintOffset(level);
    fputs("   |\n", stderr);
    _jPrintOffset(level + 2);
    fprintf(stderr, "hash = 0x%lx\n", id->hash);
    */

    _jPrintOffset(level);
    fputs("   |\n", stderr);
    _jPrintOffset(level + 2);
    fprintf(stderr, "mut = %s\n", id->is_mutable ? "true" : "false");
}

void               _jPrintBinExpr(const jNodeExprBin*, u64);

static inline void _jPrintExpr(const jNodeExpression* expr, u64 level) {
    if (!expr) {
        fputs("(NULL)\n", stderr);
        return;
    }

    _jPrintOffset(level);
    fputs("expr:\n", stderr);
    _jPrintOffset(level);
    fputs("   \\\n", stderr);

    switch (expr->has) {
    case JN_LITERAL: _jPrintLit(expr->lit, level + 2); return;
    case JN_EXPR_ID: _jPrintId(expr->id, level + 2); return;
    case JN_EXPR_BIN: _jPrintBinExpr(expr->bin_expr, level + 2); return;

    default: return;
    }
}

void _jPrintBinExpr(const jNodeExprBin* bin_expr, u64 level) {
    _jPrintOffset(level);
    fputs("bin_expr:\n", stderr);

    _jPrintOffset(level);
    fputs("   \\\n", stderr);
    _jPrintOp(bin_expr->op, level + 2);

    _jPrintOffset(level);
    fputs("   |\n", stderr);
    _jPrintExpr(bin_expr->lhs, level + 2);

    _jPrintOffset(level);
    fputs("   |\n", stderr);
    _jPrintExpr(bin_expr->rhs, level + 2);
}

static inline void _jPrintInit(const jNodeStatInit* init, u64 level) {
    _jPrintOffset(level);
    fputs("init:\n", stderr);

    _jPrintOffset(level);
    fputs("   \\\n", stderr);
    _jPrintType(init->type, level + 2);

    _jPrintOffset(level);
    fputs("   |\n", stderr);
    _jPrintId(init->id, level + 2);

    _jPrintOffset(level);
    fputs("   |\n", stderr);
    _jPrintExpr(init->expr, level + 2);
}

static inline void _jPrintExit(const jNodeStatExit* exit, u64 level) {
    _jPrintOffset(level);
    fputs("exit:\n", stderr);

    _jPrintOffset(level);
    fputs("   \\\n", stderr);
    _jPrintExpr(exit->expr, level + 2);
}

static inline void _jPrintStmt(const jNodeStatement* stmt, u64 level) {
    fputs("stmt:\n", stderr);
    fputs("   \\\n", stderr);

    switch (stmt->has) {
    case JN_STAT_EXIT: _jPrintExit(stmt->exit, level + 2); return;
    case JN_STAT_INIT: _jPrintInit(stmt->init, level + 2); return;

    default: return;
    }
}

static inline void _jDebugProgramTree(jNodeStatement* stmts) {
    fputs("\nPROGRAM TREE DEBUG PRINT:\n\n", stderr);

    for (u64 i = 0; i < J_ARRAY_SIZE(stmts); ++i) {
        _jPrintStmt(&stmts[i], 0);
        fputs("\n", stderr);
    }
}

#endif
