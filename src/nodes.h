#ifndef _NODE_H_
#define _NODE_H_

#include "tokens.h"
#include "types.h"

enum j_node_t {
    JN_INVALID = 0,

    JN_STMT,
    JN_STMT_EXIT,
    JN_STMT_INIT,

    JN_BIN_EXPR,
    JN_EXPR,
    JN_EXPR_ID,
    JN_EXPR_FSTR,

    JN_LIT,
    JN_LIT_INT,
    JN_LIT_FLOAT,
    JN_LIT_STR,
};

typedef struct jNodeRoot     jNodeRoot;

typedef struct jNodeStmt     jNodeStmt;
typedef struct jNodeStmtExit jNodeStmtExit;
typedef struct jNodeStmtInit jNodeStmtInit;

typedef struct jNodeBinExpr  jNodeBinExpr;
typedef struct jNodeExpr     jNodeExpr;
typedef struct jNodeExprId   jNodeExprId;
typedef struct jNodeExprFStr jNodeExprFStr;
typedef struct jNodeLit      jNodeLit;

struct jNodeRoot {
    jNodeStmt* stmts;
    u64        stmt_count;
};

struct jNodeStmt {
    union {
        jNodeStmtExit* exit;
        jNodeStmtInit* init;
    };

    enum j_node_t has;
};

struct jNodeStmtExit {
    jNodeExpr* expr;
};

struct jNodeStmtInit {
    enum j_token_t type;
    jNodeExprId*   id;
    jNodeExpr*     expr;
};

struct jNodeBinExpr {
    jNodeExpr*     lhs;
    jNodeExpr*     rhs;
    enum j_token_t op;
};

struct jNodeExpr {
    union {
        jNodeLit*      lit;
        jNodeBinExpr*  bin_expr;
        jNodeExprId*   id;
        jNodeExprFStr* fstr;
    };

    enum j_node_t has;
};

struct jNodeExprFStr {
    u64         ids[64];
    const char* fmt;
    u64         id_count;
};

struct jNodeExprId {
    const char* id;
    u64         hash;
    b8 mutable;
};

struct jNodeLit {
    union {
        i64         int_value;
        f64         float_value;
        const char* str_value;
    };

    enum j_node_t has;
};

#endif
