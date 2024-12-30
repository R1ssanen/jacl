#ifndef _PARSE_H_
#define _PARSE_H_

#include <string.h>

#include "error.h"
#include "nodes.h"
#include "tokens.h"
#include "types.h"

typedef struct {
    jToken* tokens;
    u64     curr_ptr;

    //  NOTE: TEMPORARY!
    jNodeStmtInit vars[256];
    u64           var_count;
} jParser;

//  NOTE: TEMPORARY! MOVE STACK VARIABLES TO OWN STRUCT!!!
static inline jNodeExpr* _jGetVarExpr(const jNodeExprId* id, const jParser* parser) {
    for (u64 i = 0; i < parser->var_count; ++i) {
        const jNodeStmtInit* var = &parser->vars[i];
        if (var->id->hash == id->hash) { return var->expr; }
    }

    return NULL;
}

b8 jParse(jParser* parser, jNodeStmt* stmts, jErrorHandler* err);

#endif
