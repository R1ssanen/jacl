#ifndef _TOKENIZE_H_
#define _TOKENIZE_H_

#include "tokens.h"
#include "types.h"

typedef struct {
    const u8* src;
    u64       src_len;
    u64       pos;
    u64       line;
    u64       column;
    u64       token_start;
    u8        curr, next;
} jLexer;

void jTokenize(jLexer* lexer, jToken** tokens);

#endif
