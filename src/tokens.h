#ifndef _TOKENS_H_
#define _TOKENS_H_

#include "types.h"

#define _J_MS_BIT(n) (1 << (14 - n))

enum j_token_t {
    JT_INVALID     = 0,
    JT_EOF         = 1,

    // OPERATORS
    JT_OP          = _J_MS_BIT(0),
    JT_OP_ADD      = (1 | JT_OP),
    JT_OP_SUB      = (2 | JT_OP),
    JT_OP_MUL      = (3 | JT_OP),
    JT_OP_DIV      = (4 | JT_OP),
    JT_OP_EQ       = (5 | JT_OP),
    JT_OP_LESS     = (6 | JT_OP),
    JT_OP_GREAT    = (7 | JT_OP),
    JT_OP_AND      = (8 | JT_OP),
    JT_OP_OR       = (9 | JT_OP),

    // SPECIAL CHARACTERS
    JT_SPEC        = _J_MS_BIT(1),
    JT_SPEC_LBRACK = (1 | JT_SPEC),
    JT_SPEC_RBRACK = (2 | JT_SPEC),
    JT_SPEC_LPAREN = (3 | JT_SPEC),
    JT_SPEC_RPAREN = (4 | JT_SPEC),
    JT_SPEC_LBRACE = (5 | JT_SPEC),
    JT_SPEC_RBRACE = (6 | JT_SPEC),
    JT_SPEC_DOT    = (7 | JT_SPEC),
    JT_SPEC_COMMA  = (8 | JT_SPEC),
    JT_SPEC_COLON  = (9 | JT_SPEC),
    JT_SPEC_SEMI   = (10 | JT_SPEC),
    JT_SPEC_FORMAT = (11 | JT_SPEC),

    // KEYWORDS
    JT_KEY         = _J_MS_BIT(2),
    JT_KEY_RET     = (1 | JT_KEY),
    JT_KEY_IF      = (2 | JT_KEY),
    JT_KEY_ELIF    = (3 | JT_KEY),
    JT_KEY_ELSE    = (4 | JT_KEY),
    JT_KEY_MUT     = (5 | JT_KEY),
    JT_KEY_EXIT    = (6 | JT_KEY),
    JT_KEY_FOR     = (7 | JT_KEY),
    JT_KEY_CONT    = (8 | JT_KEY),
    JT_KEY_WHILE   = (9 | JT_KEY),

    // ELEMENTARY TYPES
    JT_TYPE        = _J_MS_BIT(3),
    JT_TYPE_B8     = (1 | JT_TYPE),
    JT_TYPE_I8     = (2 | JT_TYPE),
    JT_TYPE_I16    = (3 | JT_TYPE),
    JT_TYPE_I32    = (4 | JT_TYPE),
    JT_TYPE_I64    = (5 | JT_TYPE),
    JT_TYPE_U8     = (6 | JT_TYPE),
    JT_TYPE_U16    = (7 | JT_TYPE),
    JT_TYPE_U32    = (8 | JT_TYPE),
    JT_TYPE_U64    = (9 | JT_TYPE),
    JT_TYPE_F32    = (10 | JT_TYPE),
    JT_TYPE_F64    = (11 | JT_TYPE),

    // LITERALS
    JT_LIT         = _J_MS_BIT(4),
    JT_LIT_INT     = (1 | JT_LIT),
    JT_LIT_FLOAT   = (2 | JT_LIT),
    JT_LIT_STR     = (3 | JT_LIT),

    // IDENTIFIER
    JT_ID          = _J_MS_BIT(5),
};

void           jPrehashTokenStrings(void);
enum j_token_t jTryGetToken(const u8* str, u64 len);
const char*    jGetTokenString(enum j_token_t type);

typedef struct {
    u8 lbp, rbp;
} _jOpInfo;

typedef struct {
    _jOpInfo       opinfo; // for operators
    u8*            data;
    u64            line, column, length;
    enum j_token_t type;
} jToken;

void jPrintDebugToken(const jToken* token);

#endif
