#include "tokenize.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "tokens.h"
#include "types.h"

_jOpInfo _J_OPINFOS[] = {
    [JT_OP_OR] = { 1, 2 },
      [JT_OP_AND] = { 2, 3 },
      [JT_OP_EQ] = { 3, 4 },
      [JT_OP_ADD] = { 4, 5 },
    [JT_OP_SUB] = { 4, 5 },
      [JT_OP_MUL] = { 5, 6 },
      [JT_OP_DIV] = { 5, 6 },
};

b8 jLexerIsEOF(jLexer* lexer) {
    if ((lexer->pos >= lexer->src_len) || (lexer->src[lexer->pos] == '\0')) { return true; }
    return false;
}

char jLexerPeek(jLexer* lexer, u64 count) {
    if (jLexerIsEOF(lexer)) { return '\0'; }
    return lexer->src[lexer->pos];
}

char jLexerConsume(jLexer* lexer) {
    char c = jLexerPeek(lexer, 0);
    if (!c) { return c; }

    if (c == '\n') {
        lexer->line++;
        lexer->col = 0;
    } else {
        lexer->col++;
    }

    lexer->pos++;
    return c;
}

b8 _IsDigit(char c, b8 include_decimal) {
    const char* DIGIT    = "1234567890";
    b8          is_digit = strchr(DIGIT, c);
    if (!include_decimal) { return is_digit; }
    return is_digit || (c == '.');
}

b8 _IsValidId(char c, b8 is_begin) {
    const char* ALPHA_PLUS_UNDER = "_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    b8          is_alpha         = strchr(ALPHA_PLUS_UNDER, c);
    if (is_begin) { return is_alpha; }
    return is_alpha || _IsDigit(c, false);
}

b8 _IsOperator(char c) {
    const char* OPERATOR = "+-*/=<>&|";
    return strchr(OPERATOR, c);
}

b8 _IsSpecial(char c) {
    const char* SPECIAL = "[](){}.,:;f";
    return strchr(SPECIAL, c);
}

void jTokenize(jLexer* lexer, jToken* tokens) {
    jPrehashTokenStrings();

    char c;
    while ((c = jLexerPeek(lexer, 0))) {

        // comment
        if (c == '$') {
            jLexerConsume(lexer); // first '$'
            char end = (jLexerConsume(lexer) == '$') ? '\n' : '$';
            while (jLexerConsume(lexer) != end);
        }

        // special character / operator
        else if (_IsSpecial(c) || _IsOperator(c)) {
            jToken token = {
                .line = lexer->line,
                .col  = lexer->col,
                .type = jTryGetToken(&c, 1),
            };

            J_ARRAY_PUSH(tokens, token);
            jLexerConsume(lexer);
        }

        // identifier / keyword
        else if (_IsValidId(c, true)) {
            u64 start = lexer->pos;
            while (_IsValidId(jLexerPeek(lexer, 1), false)) { jLexerConsume(lexer); }

            u64   len = lexer->pos - start;
            char* str = malloc((len + 1) * sizeof(char));
            strncpy(str, lexer->src + start, len);
            str[len]             = '\0';

            enum j_token_t type  = jTryGetToken(str, len);
            jToken         token = {
                        .str  = str,
                        .line = lexer->line,
                        .col  = lexer->col,
                        .type = type ? type : JT_ID,
            };

            J_ARRAY_PUSH(tokens, token);
        }

        // numeric literal
        else if (_IsDigit(c, false)) {
            u64 start    = lexer->pos;
            b8  is_float = false;
            while (_IsDigit(jLexerPeek(lexer, 1), true)) {
                if (jLexerConsume(lexer) == '.') { is_float = true; }
            }

            u64  len = lexer->pos - start;
            char data[len + 1];
            strncpy(data, lexer->src + start, len);
            data[len]    = '\0';

            jToken token = {
                .int_value = strtol(data, NULL, 10),
                .line      = lexer->line,
                .col       = lexer->col,
                .type      = JT_LIT_INT,
            };

            if (is_float) {
                token.float_value = strtof(data, NULL);
                token.type        = JT_LIT_FLOAT;
            } else {
                token.int_value = strtol(data, NULL, 10);
                token.type      = JT_LIT_INT;
            }

            J_ARRAY_PUSH(tokens, token);
        }

        // string literal
        else if (c == '\"') {
            jLexerConsume(lexer); // first '"'
            u64 start = lexer->pos;
            while (jLexerPeek(lexer, 1) != '\"') { jLexerConsume(lexer); }

            u64 len = lexer->pos - start;
            jLexerConsume(lexer); // second '"'

            char* str = malloc((len + 1) * sizeof(char));
            strncpy(str, lexer->src + start, len);
            str[len]     = '\0';

            jToken token = {
                .str  = str,
                .line = lexer->line,
                .col  = lexer->col,
                .type = JT_LIT_STR,
            };

            J_ARRAY_PUSH(tokens, token);
        }

        else {
            jLexerConsume(lexer);
        }
    }
}
