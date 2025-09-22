#ifndef JAC_LEX_H_
#define JAC_LEX_H_

#include "darray.h"

typedef struct jac_token_diagnostics jac_token_diagnostics;
typedef struct jac_token jac_token;

enum jac_token_kind
{
    JAC_TOKEN_EOF = '\0',
    JAC_TOKEN_IDENTIFIER,
    JAC_TOKEN_INT_LITERAL,
    JAC_TOKEN_STR_LITERAL,

    JAC_TOKEN_RETURN,
    JAC_TOKEN_EXTERN,
    JAC_TOKEN_FUNC,
    JAC_TOKEN_INT32,
    JAC_TOKEN_UINT8,

    JAC_TOKEN_SEMICOLON = ';',
    JAC_TOKEN_COLON = ':',
    JAC_TOKEN_COMMA = ',',
    JAC_TOKEN_LBRACE = '{',
    JAC_TOKEN_RBRACE = '}',
    JAC_TOKEN_LPAREN = '(',
    JAC_TOKEN_RPAREN = ')',
    JAC_TOKEN_STAR = '*',
    JAC_TOKEN_MINUS = '-',
    JAC_TOKEN_GT = '>',
    JAC_TOKEN_AMPERSAND = '&',
    JAC_TOKEN_EQUALS = '=',
};

struct jac_token_diagnostics
{
    const char *source_line;
    size_t length;
    size_t line;
    size_t column;
};

struct jac_token
{
    jac_token_diagnostics diagnostics;
    darray_t char *value;
    enum jac_token_kind kind;
};

// NOTE: expects null-termination
darray_t jac_token *jac_tokenize(const char *source);

void jac_free_tokens(darray_t jac_token *tokens);

#endif
