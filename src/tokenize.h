#ifndef _TOKENIZE_H_
#define _TOKENIZE_H_

#include "tokens.h"
#include "types.h"

typedef struct {
    const char* src;
    u64         src_len;
    u64         pos;
    u64         line, col;
} jLexer;

void jTokenize(jLexer* lexer, jToken* tokens);

#endif
