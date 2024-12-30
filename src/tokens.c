#include "tokens.h"

#include <stdio.h>
#include <string.h>

#include "hash.h"

typedef struct {
    const char*    str;
    u64            hash;
    enum j_token_t type;
} jTokenPair;

#define _J_TOKEN_PAIR(s, t)                                                                        \
    (jTokenPair) { .str = s, .hash = 0, .type = t }

jTokenPair _J_TOKEN_STRINGS[] = {
    // OPERATORS
    // arithmetic
    (jTokenPair){ "+",    0, JT_OP_ADD      },
    (jTokenPair){ "-",    0, JT_OP_SUB      },
    (jTokenPair){ "*",    0, JT_OP_MUL      },
    (jTokenPair){ "/",    0, JT_OP_DIV      },

    // logical
    (jTokenPair){ "=",    0, JT_OP_EQ       },
    (jTokenPair){ "<",    0, JT_OP_LESS     },
    (jTokenPair){ ">",    0, JT_OP_GREAT    },
    (jTokenPair){ "&",    0, JT_OP_AND      },
    (jTokenPair){ "|",    0, JT_OP_OR       },

    // other
    (jTokenPair){ ":=",   0, JT_OP_ASSIGN   },

    // SPECIAL CHARACTERS
    (jTokenPair){ "[",    0, JT_SPEC_LBRACK },
    (jTokenPair){ "]",    0, JT_SPEC_RBRACK },
    (jTokenPair){ "(",    0, JT_SPEC_LPAREN },
    (jTokenPair){ ")",    0, JT_SPEC_RPAREN },
    (jTokenPair){ "{",    0, JT_SPEC_LBRACE },
    (jTokenPair){ "}",    0, JT_SPEC_RBRACE },
    (jTokenPair){ ".",    0, JT_SPEC_DOT    },
    (jTokenPair){ ",",    0, JT_SPEC_COMMA  },
    (jTokenPair){ ":",    0, JT_SPEC_COLON  },
    (jTokenPair){ ";",    0, JT_SPEC_SEMI   },
    (jTokenPair){ "f",    0, JT_SPEC_FORMAT },

    // KEYWORDS
    (jTokenPair){ "exit", 0, JT_KEY_EXIT    },

    // ELEMENTARY TYPES
    (jTokenPair){ "b8",   0, JT_TYPE_B8     },
    (jTokenPair){ "i8",   0, JT_TYPE_I8     },
    (jTokenPair){ "i32",  0, JT_TYPE_I32    },
    (jTokenPair){ "i64",  0, JT_TYPE_I64    },
    (jTokenPair){ "u8",   0, JT_TYPE_U8     },
    (jTokenPair){ "u32",  0, JT_TYPE_U32    },
    (jTokenPair){ "u64",  0, JT_TYPE_U64    },
    (jTokenPair){ "f32",  0, JT_TYPE_F32    },
    (jTokenPair){ "f64",  0, JT_TYPE_F64    },
};

void jPrehashTokenStrings(void) {
    u64 tokens = sizeof(_J_TOKEN_STRINGS) / sizeof(jTokenPair);

    for (u64 i = 0; i < tokens; ++i) {
        jTokenPair* pair = _J_TOKEN_STRINGS + i;
        if (!pair->str) { fputs("what\n", stderr); }
        pair->hash = FNV_1A(pair->str, strlen(pair->str));
    }
}

enum j_token_t jTryGetToken(const char* str, u64 len) {
    if (!str) { return JT_INVALID; }

    u64 hash = FNV_1A(str, len);
    if (hash == 0ULL) { return JT_INVALID; }

    u64 tokens = sizeof(_J_TOKEN_STRINGS) / sizeof(jTokenPair);

    for (u64 i = 0; i < tokens; ++i) {
        jTokenPair* pair = _J_TOKEN_STRINGS + i;
        if (pair->hash == hash) { return pair->type; }
    }

    return JT_INVALID;
}

const char* jGetTokenString(enum j_token_t type) {
    u64 tokens = sizeof(_J_TOKEN_STRINGS) / sizeof(jTokenPair);

    for (u64 i = 0; i < tokens; ++i) {
        jTokenPair* pair = _J_TOKEN_STRINGS + i;
        if (pair->type == type) { return pair->str; }
    }

    return NULL;
}

void jPrintDebugToken(const jToken* token) {
    if (token->type & JT_ID) {
        fprintf(stderr, "(%lu, %lu) id: %s\n", token->line, token->col, token->str);
    }

    else if (token->type & JT_OP) {
        fprintf(
            stderr, "(%lu, %lu) op: %s\n", token->line, token->col, jGetTokenString(token->type)
        );
    }

    else if (token->type & JT_SPEC) {
        fprintf(
            stderr, "(%lu, %lu) spec: %s\n", token->line, token->col, jGetTokenString(token->type)
        );
    }

    else if (token->type & JT_TYPE) {
        fprintf(stderr, "(%lu, %lu) type: %s\n", token->line, token->col, token->str);
    }

    else if (token->type & JT_KEY) {
        fprintf(stderr, "(%lu, %lu) key: %s\n", token->line, token->col, token->str);
    }

    else if (token->type & JT_LIT) {
        if (token->type == JT_LIT_INT) {
            fprintf(stderr, "(%lu, %lu) int: %lli\n", token->line, token->col, token->int_value);
        } else if (token->type == JT_LIT_FLOAT) {
            fprintf(stderr, "(%lu, %lu) float: %f\n", token->line, token->col, token->float_value);
        } else {
            fprintf(stderr, "(%lu, %lu) str: %s\n", token->line, token->col, token->str);
        }
    }

    else {
        fputs("unknown: none", stderr);
    }
}
