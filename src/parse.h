#ifndef _PARSE_H_
#define _PARSE_H_

#include "error.h"
#include "nodes.h"
#include "tokens.h"

enum j_error_t jParse(jToken* tokens, u64 token_count, jNodeRoot* program);

#endif
