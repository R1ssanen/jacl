#ifndef _TOKENS_H_
#define _TOKENS_H_

#include "types.h"

#define _J_MS_BIT(n) (1 << (30 - n))

enum j_token_t {
    JT_INVALID     = 0,

    // operators
    JT_OP          = _J_MS_BIT(0),
    JT_OP_ADD      = (0 | JT_OP),
    JT_OP_SUB      = (1 | JT_OP),
    JT_OP_MUL      = (2 | JT_OP),
    JT_OP_DIV      = (3 | JT_OP),
    JT_OP_EQ       = (4 | JT_OP),
    JT_OP_LESS     = (5 | JT_OP),
    JT_OP_GREAT    = (6 | JT_OP),
    JT_OP_AND      = (7 | JT_OP),
    JT_OP_OR       = (8 | JT_OP),

    // special characters
    JT_SPEC        = _J_MS_BIT(1),
    JT_SPEC_LBRACK = (0 | JT_SPEC),
    JT_SPEC_RBRACK = (1 | JT_SPEC),
    JT_SPEC_LPAREN = (2 | JT_SPEC),
    JT_SPEC_RPAREN = (3 | JT_SPEC),
    JT_SPEC_LBRACE = (4 | JT_SPEC),
    JT_SPEC_RBRACE = (5 | JT_SPEC),
    JT_SPEC_DOT    = (6 | JT_SPEC),
    JT_SPEC_COMMA  = (7 | JT_SPEC),
    JT_SPEC_COLON  = (8 | JT_SPEC),
    JT_SPEC_SEMI   = (9 | JT_SPEC),
    JT_SPEC_FORMAT = (10 | JT_SPEC),

    // keywords
    JT_KEY         = _J_MS_BIT(2),
    JT_KEY_TRUE    = (0 | JT_KEY),
    JT_KEY_FALSE   = (1 | JT_KEY),
    JT_KEY_RET     = (2 | JT_KEY),
    JT_KEY_IF      = (3 | JT_KEY),
    JT_KEY_ELIF    = (4 | JT_KEY),
    JT_KEY_ELSE    = (5 | JT_KEY),
    JT_KEY_MUT     = (6 | JT_KEY),
    JT_KEY_EXIT    = (7 | JT_KEY),
    JT_KEY_FOR     = (8 | JT_KEY),
    JT_KEY_CONT    = (9 | JT_KEY),
    JT_KEY_WHILE   = (10 | JT_KEY),

    // built-in types
    JT_TYPE        = _J_MS_BIT(3),
    JT_TYPE_B8     = (0 | JT_TYPE),
    JT_TYPE_I8     = (1 | JT_TYPE),
    JT_TYPE_I32    = (2 | JT_TYPE),
    JT_TYPE_I64    = (3 | JT_TYPE),
    JT_TYPE_U8     = (4 | JT_TYPE),
    JT_TYPE_U32    = (5 | JT_TYPE),
    JT_TYPE_U64    = (6 | JT_TYPE),
    JT_TYPE_F32    = (7 | JT_TYPE),
    JT_TYPE_F64    = (8 | JT_TYPE),

    // literals
    JT_LIT         = _J_MS_BIT(4),
    JT_LIT_INT     = (0 | JT_LIT),
    JT_LIT_FLOAT   = (1 | JT_LIT),
    JT_LIT_STR     = (2 | JT_LIT),

    // identifier
    JT_ID,
};

enum j_token_t jTryGetToken(const char* str, b8* hashed);
const char*    jGetTokenString(enum j_token_t type);

typedef struct {
    enum {
        J_ASSOC_LEFT,
        J_ASSOC_RIGHT,
    } assoc;

    u8 prec;
} _jOpInfo;

typedef struct {
    union {
        _jOpInfo    opinfo;      // for operator
        const char* str;         // for identifier and str literal
        i64         int_value;   // for int literal
        f64         float_value; // for float literal
    };

    u64            line;
    enum j_token_t type;
} jToken;

void jPrintDebugToken(const jToken* token);

#endif
