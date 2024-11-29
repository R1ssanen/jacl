#include "tokens.h"

#include <stdio.h>
#include <string.h>

#include "hash.h"

typedef struct {
    const char*    str;
    u64            hash;
    enum j_token_t type;
} jLongPair;

#define _J_LONG_PAIR(str, type) [type] = { str, 0, type }

jLongPair _J_LONG_TOKENS[] = {
    // keywords
    _J_LONG_PAIR("exit", JT_KEY_EXIT),

    // built-in types
    _J_LONG_PAIR("b8", JT_TYPE_B8),
    _J_LONG_PAIR("i8", JT_TYPE_I8),
    _J_LONG_PAIR("i32", JT_TYPE_I32),
    _J_LONG_PAIR("i64", JT_TYPE_I64),
    _J_LONG_PAIR("u8", JT_TYPE_U8),
    _J_LONG_PAIR("u32", JT_TYPE_U32),
    _J_LONG_PAIR("u64", JT_TYPE_U64),
    _J_LONG_PAIR("f32", JT_TYPE_F32),
    _J_LONG_PAIR("f64", JT_TYPE_F64),
};

void jPrehashLongTokens(void) {

    u64 tokens = sizeof(_J_LONG_TOKENS) / sizeof(jLongPair);

    for (u64 i = 0; i < tokens; ++i) {
        jLongPair* pair = _J_LONG_TOKENS + i;
        pair->hash      = FNV_1A(pair->str);
    }
}

enum j_token_t jTryGetLongToken(const char* str) {

    if (!str) { return JT_INVALID; }

    u64 hash = FNV_1A(str);
    if (hash == 0ULL) { return JT_INVALID; }

    u64 tokens = sizeof(_J_LONG_TOKENS) / sizeof(jLongPair);

    for (u64 i = 0; i < tokens; ++i) {
        jLongPair* pair = _J_LONG_TOKENS + i;
        if (pair->hash == hash) { return pair->type; }
    }

    return JT_INVALID;
}

const char* jGetLongTokenString(enum j_token_t type) { return _J_LONG_TOKENS[type].str; }

typedef struct {
    enum j_token_t type;
    const char     c;
} jShortPair;

#define _J_SHORT_PAIR(c, type) [type] = { type, c }

jShortPair _J_SHORT_TOKENS[] = {
    // operators
    _J_SHORT_PAIR('+', JT_OP_ADD),
    _J_SHORT_PAIR('-', JT_OP_SUB),
    _J_SHORT_PAIR('*', JT_OP_MUL),
    _J_SHORT_PAIR('/', JT_OP_DIV),
    _J_SHORT_PAIR('=', JT_OP_EQ),
    _J_SHORT_PAIR('<', JT_OP_LESS),
    _J_SHORT_PAIR('>', JT_OP_GREAT),
    _J_SHORT_PAIR('&', JT_OP_AND),
    _J_SHORT_PAIR('|', JT_OP_OR),

    // special characters
    _J_SHORT_PAIR('[', JT_SPEC_LBRACK),
    _J_SHORT_PAIR(']', JT_SPEC_RBRACK),
    _J_SHORT_PAIR('(', JT_SPEC_LPAREN),
    _J_SHORT_PAIR(')', JT_SPEC_RPAREN),
    _J_SHORT_PAIR('{', JT_SPEC_LBRACE),
    _J_SHORT_PAIR('}', JT_SPEC_RBRACE),
    _J_SHORT_PAIR('.', JT_SPEC_DOT),
    _J_SHORT_PAIR(',', JT_SPEC_COMMA),
    _J_SHORT_PAIR(':', JT_SPEC_COLON),
    _J_SHORT_PAIR(';', JT_SPEC_SEMI),
    _J_SHORT_PAIR('f', JT_SPEC_FORMAT),
};

enum j_token_t jTryGetShortToken(char c) {

    u64 tokens = sizeof(_J_SHORT_TOKENS) / sizeof(jShortPair);

    for (u64 i = 0; i < tokens; ++i) {
        jShortPair* pair = _J_SHORT_TOKENS + i;
        if (pair->c == c) { return pair->type; }
    }

    return JT_INVALID;
}

char jGetShortTokenCharacter(enum j_token_t type) { return _J_SHORT_TOKENS[type].c; }

void jPrintDebugToken(const jToken* token) {

    if (token->type & JT_ID) {
        fprintf(stderr, "id: %s\n", token->str);
    }

    else if (token->type & JT_OP) {
        fprintf(stderr, "op: %c\n", jGetShortTokenCharacter(token->type));
    }

    else if (token->type & JT_SPEC) {
        fprintf(stderr, "spec: %c\n", jGetShortTokenCharacter(token->type));
    }

    else if (token->type & JT_TYPE) {
        fprintf(stderr, "type: %s\n", token->str);
    }

    else if (token->type & JT_KEY) {
        fprintf(stderr, "key: %s\n", token->str);
    }

    else if (token->type & JT_LIT) {
        if (token->type == JT_LIT_INT) {
            fprintf(stderr, "int: %lli\n", token->int_value);
        } else if (token->type == JT_LIT_FLOAT) {
            fprintf(stderr, "float: %f\n", token->float_value);
        } else {
            fprintf(stderr, "str: %s\n", token->str);
        }
    }

    else {
        fputs("unknown: none", stderr);
    }
}
