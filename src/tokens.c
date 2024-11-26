#include "tokens.h"

#include <stdio.h>
#include <string.h>

#include "hash.h"

typedef struct {
    const char*    str;
    u64            hash;
    enum j_token_t type;
} jTypePair;

#define _J_MAKE_PAIR(str, type) [type] = { str, type }

jTypePair _J_TOKENS[] = {
    // operators
    _J_MAKE_PAIR("+", JT_OP_ADD),
    _J_MAKE_PAIR("-", JT_OP_SUB),
    _J_MAKE_PAIR("*", JT_OP_MUL),
    _J_MAKE_PAIR("/", JT_OP_DIV),
    _J_MAKE_PAIR("=", JT_OP_EQ),
    _J_MAKE_PAIR("<", JT_OP_LESS),
    _J_MAKE_PAIR(">", JT_OP_GREAT),
    _J_MAKE_PAIR("&", JT_OP_AND),
    _J_MAKE_PAIR("|", JT_OP_OR),

    // special symbols
    _J_MAKE_PAIR("[", JT_SPEC_LBRACK),
    _J_MAKE_PAIR("]", JT_SPEC_RBRACK),
    _J_MAKE_PAIR("(", JT_SPEC_LPAREN),
    _J_MAKE_PAIR(")", JT_SPEC_RPAREN),
    _J_MAKE_PAIR("{", JT_SPEC_LBRACE),
    _J_MAKE_PAIR("}", JT_SPEC_RBRACE),
    _J_MAKE_PAIR(".", JT_SPEC_DOT),
    _J_MAKE_PAIR(",", JT_SPEC_COMMA),
    _J_MAKE_PAIR(":", JT_SPEC_COLON),
    _J_MAKE_PAIR(";", JT_SPEC_SEMI),
    _J_MAKE_PAIR("f", JT_SPEC_FORMAT),

    // keywords
    _J_MAKE_PAIR("exit", JT_KEY_EXIT),

    // built-in types
    _J_MAKE_PAIR("b8", JT_TYPE_B8),
    _J_MAKE_PAIR("i8", JT_TYPE_I8),
    _J_MAKE_PAIR("i32", JT_TYPE_I32),
    _J_MAKE_PAIR("i64", JT_TYPE_I64),
    _J_MAKE_PAIR("u8", JT_TYPE_U8),
    _J_MAKE_PAIR("u32", JT_TYPE_U32),
    _J_MAKE_PAIR("u64", JT_TYPE_U64),
    _J_MAKE_PAIR("f32", JT_TYPE_F32),
    _J_MAKE_PAIR("f64", JT_TYPE_F64),
};

enum j_token_t jTryGetToken(const char* str, b8* hashed) {
    if (!str) { return JT_INVALID; }

    u64 tokens = sizeof(_J_TOKENS) / sizeof(jTypePair);
    if (!(*hashed)) {
        for (u64 i = 0; i < tokens; ++i) {
            jTypePair* pair = &_J_TOKENS[i];
            pair->hash      = FNV_1A(pair->str);
        }

        *hashed = true;
    }

    u64 hash = FNV_1A(str);

    for (u64 i = 0; i < tokens; ++i) {
        jTypePair* pair = &_J_TOKENS[i];
        if (pair->hash == hash) { return pair->type; }
    }

    return JT_INVALID;
}

const char* jGetTokenString(enum j_token_t type) { return _J_TOKENS[type].str; }

/*void jPrintDebugToken(const jToken* token) {

    switch (token->class) {

    case JC_ID: fprintf(stderr, "id: %s\n", token->str); return;

    case JC_OP: fprintf(stderr, "op: %c\n", token->type); return;

    case JC_LIT: {
        if (token->type == JT_LIT_INT) {
            fprintf(stderr, "int: %li\n", token->int_value);
        } else if (token->type == JT_LIT_FLOAT) {
            fprintf(stderr, "float: %f\n", token->float_value);
        } else {
            fprintf(stderr, "str: %s\n", token->str);
        }
        return;
    }

    case JC_SPEC: fprintf(stderr, "spec: %c\n", token->type); return;

    case JC_TYPE: fprintf(stderr, "type: %s\n", token->str); return;

    case JC_KEY: fprintf(stderr, "key: %s\n", token->str); return;

    default: return;
    }
}*/
