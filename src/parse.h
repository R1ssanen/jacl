#ifndef _PARSE_H_
#define _PARSE_H_

#include "error.h"
#include "nodes.h"
#include "tokens.h"

typedef struct {
    jToken* tokens;
    u64     token_count;
    u64     consumed;

    //  NOTE: TEMPORARY!
    jNodeStmtInit vars[256];
    u64           var_count;
} jParser;

enum j_error_t jParse(jParser* parser, jNodeRoot* program);

#endif
