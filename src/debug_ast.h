#ifndef _DEBUG_AST_H_
#define _DEBUG_AST_H_

#include <stdio.h>

#include "nodes.h"
#include "tokens.h"

static inline void _jPrintOffset(i32 offset) { fprintf(stderr, "%*s", offset, ""); }

static inline void _jPrintType(enum j_token_t type, u64 level) {
    _jPrintOffset(level);
    fprintf(stderr, "type = %s\n", jGetLongTokenString(type));
}

static inline void _jPrintOp(enum j_token_t op, u64 level) {
    _jPrintOffset(level);
    fprintf(stderr, "op = (%c)\n", op);
}

static inline void _jPrintFloat(f64 num, u64 level) {
    _jPrintOffset(level);
    fprintf(stderr, "float = %f\n", num);
}

static inline void _jPrintInt(i64 num, u64 level) {
    _jPrintOffset(level);
    fprintf(stderr, "int = %lli\n", num);
}

static inline void _jPrintStr(const char* str, u64 level) {
    _jPrintOffset(level);
    fprintf(stderr, "str = '%s'\n", str);
}

static inline void _jPrintLit(const jNodeLit* lit, u64 level) {
    _jPrintOffset(level);
    fputs("lit:\n", stderr);
    _jPrintOffset(level);
    fputs("   \\\n", stderr);

    switch (lit->has) {
    case JN_LIT_INT: _jPrintInt(lit->int_value, level + 2); return;
    case JN_LIT_FLOAT: _jPrintFloat(lit->float_value, level + 2); return;
    case JN_LIT_STR: _jPrintStr(lit->str_value, level + 2); return;

    default: return;
    }
}

static inline void _jPrintFStr(const jNodeExprFStr* fstr, u64 level) {
    _jPrintOffset(level);
    fputs("fstr:\n", stderr);
    _jPrintOffset(level);
    fputs("   \\\n", stderr);

    _jPrintStr(fstr->fmt, level + 2);
    _jPrintOffset(level);
    fputs("   |\n", stderr);
    _jPrintOffset(level + 2);
    fputs("ids: [\n", stderr);

    for (u64 i = 0; i < fstr->id_count; ++i) {
        _jPrintOffset(level + 4);
        fprintf(stderr, "0x%lx\n", fstr->ids[i]);
    }

    _jPrintOffset(level + 2);
    fputs("]\n", stderr);
}

static inline void _jPrintId(jNodeExprId* id, u64 level) {
    _jPrintOffset(level);
    fputs("id:\n", stderr);

    _jPrintOffset(level);
    fputs("   \\\n", stderr);
    _jPrintOffset(level + 2);
    fprintf(stderr, "ident = '%s'\n", id->id);

    _jPrintOffset(level);
    fputs("   |\n", stderr);
    _jPrintOffset(level + 2);
    fprintf(stderr, "hash = 0x%lx\n", id->hash);

    _jPrintOffset(level);
    fputs("   |\n", stderr);
    _jPrintOffset(level + 2);
    fprintf(stderr, "mut = %s\n", id->hash ? "true" : "false");
}

void               _jPrintBinExpr(const jNodeBinExpr*, u64);

static inline void _jPrintExpr(const jNodeExpr* expr, u64 level) {
    if (!expr) {
        fputs("(NULL)\n", stderr);
        return;
    }

    _jPrintOffset(level);
    fputs("expr:\n", stderr);
    _jPrintOffset(level);
    fputs("   \\\n", stderr);

    switch (expr->has) {
    case JN_LIT: _jPrintLit(expr->lit, level + 2); return;
    case JN_EXPR_ID: _jPrintId(expr->id, level + 2); return;
    case JN_BIN_EXPR: _jPrintBinExpr(expr->bin_expr, level + 2); return;
    case JN_EXPR_FSTR: _jPrintFStr(expr->fstr, level + 2); return;

    default: return;
    }
}

void _jPrintBinExpr(const jNodeBinExpr* bin_expr, u64 level) {
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

static inline void _jPrintInit(const jNodeStmtInit* init, u64 level) {
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

static inline void _jPrintExit(const jNodeStmtExit* exit, u64 level) {
    _jPrintOffset(level);
    fputs("exit:\n", stderr);

    _jPrintOffset(level);
    fputs("   \\\n", stderr);
    _jPrintExpr(exit->expr, level + 2);
}

static inline void _jPrintStmt(const jNodeStmt* stmt, u64 level) {
    fputs("stmt:\n", stderr);
    fputs("   \\\n", stderr);

    switch (stmt->has) {
    case JN_STMT_EXIT: _jPrintExit(stmt->exit, level + 2); return;
    case JN_STMT_INIT: _jPrintInit(stmt->init, level + 2); return;

    default: return;
    }
}

static inline void _jDebugProgramTree(const jNodeRoot* program) {
    fputs("\nPROGRAM TREE DEBUG PRINT:\n\n", stderr);

    for (u64 i = 0; i < program->stmt_count; ++i) {
        _jPrintStmt(&program->stmts[i], 0);
        fputs("\n", stderr);
    }
}

#endif
