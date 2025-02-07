#ifndef _NODE_H_
#define _NODE_H_

#include "tokens.h"
#include "types.h"

enum j_node_t {
    JN_INVALID = 0,

    JN_STATEMENT,
    JN_STAT_EXIT,
    JN_STAT_INIT,
    JN_STAT_ASSIGN,

    JN_EXPRESSION,
    JN_EXPR_BIN,
    JN_EXPR_ID,
    JN_EXPR_FSTR,

    JN_LITERAL,
    JN_LIT_INT,
    JN_LIT_FLOAT,
    JN_LIT_STR,
};

typedef struct jNodeStatement  jNodeStatement;
typedef struct jNodeStatExit   jNodeStatExit;
typedef struct jNodeStatInit   jNodeStatInit;
typedef struct jNodeStatAssign jNodeStatAssign;

typedef struct jNodeExpression jNodeExpression;
typedef struct jNodeExprBin    jNodeExprBin;
typedef struct jNodeExprIdent  jNodeExprIdent;
typedef struct jNodeExprFStr   jNodeExprFStr;

typedef struct jNodeLiteral    jNodeLiteral;

struct jNodeStatement {
    union {
        jNodeStatExit* exit;
        jNodeStatInit* init;
    };

    enum j_node_t has;
};

struct jNodeStatExit {
    jNodeExpression* expr;
};

struct jNodeStatInit {
    jNodeExprIdent*  id;
    jNodeExpression* expr;
    enum j_token_t   type;
};

struct jNodeStatAssign {
    jNodeExprIdent*  id;
    jNodeExpression* expr;
};

struct jNodeExprBin {
    jNodeExpression* lhs;
    jNodeExpression* rhs;
    enum j_token_t   op;
};

struct jNodeExpression {
    union {
        jNodeLiteral*   lit;
        jNodeExprBin*   bin_expr;
        jNodeExprIdent* id;
        jNodeExprFStr*  fstr;
    };

    enum j_node_t has;
};

struct jNodeExprIdent {
    const u8* id;
    u64       hash;
    b8        is_mutable;
};

struct jNodeLiteral {
    union { // numeric values for optimization
        i64 int_value;
        f64 float_value;
    };

    u8*            value; // string form of the literal
    enum j_token_t intrinsic_type;
    enum j_node_t  has;
};

#endif
