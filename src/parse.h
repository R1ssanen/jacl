#ifndef _PARSE_H_
#define _PARSE_H_

#include <string.h>

#include "array.h"
#include "error.h"
#include "nodes.h"
#include "tokens.h"
#include "types.h"

typedef struct {
    jToken* tokens;
    u64     ptr;

    //  NOTE: TEMPORARY!
    jNodeStatInit* vars;
} jParser;

//  NOTE: TEMPORARY! MOVE STACK VARIABLES TO OWN STRUCT!!!
static inline jNodeStatInit* _jFindVariable(const jNodeExprIdent* id, const jParser* parser) {
    for (u64 i = 0; i < J_ARRAY_SIZE(parser->vars); ++i) {
        jNodeStatInit* var = parser->vars + i;
        if (var->id->hash == id->hash) { return var; }
    }

    return NULL;
}

b8 jParse(jParser* parser, jNodeStatement** stmts, jError** err);

#endif
