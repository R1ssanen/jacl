#ifndef JAC_LEX_H_
#define JAC_LEX_H_

#include "darray.h"

typedef enum jac_token_kind {
    JAC_TOKEN_EOF = '\0',
    JAC_TOKEN_IDENTIFIER,
    JAC_TOKEN_LITERAL,

    JAC_TOKEN_RETURN,
    JAC_TOKEN_EXTERN,
    JAC_TOKEN_FUNC,
    JAC_TOKEN_INT32,
    JAC_TOKEN_UINT8,

    JAC_TOKEN_SEMICOLON = ';',
    JAC_TOKEN_COLON     = ':',
    JAC_TOKEN_COMMA     = ',',
    JAC_TOKEN_LBRACE    = '{',
    JAC_TOKEN_RBRACE    = '}',
    JAC_TOKEN_LPAREN    = '(',
    JAC_TOKEN_RPAREN    = ')',
    JAC_TOKEN_STAR      = '*',
    JAC_TOKEN_MINUS     = '-',
    JAC_TOKEN_GT        = '>',
    JAC_TOKEN_AMPERSAND = '&',
} jac_token_kind;

typedef struct jac_token {
    darray_t char* value;
    size_t         pos, line, col;
    jac_token_kind kind;
} jac_token;

// NOTE: expects null-termination
darray_t jac_token* jac_tokenize(const char* source);

void                jac_free_tokens(darray_t jac_token* tokens);

#endif
