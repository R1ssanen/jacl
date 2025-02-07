#include "tokenize.h"

#include <ctype.h>
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

static b8 IsEOF(jLexer* lexer) {
    return (lexer->pos >= lexer->src_len) || (lexer->src[lexer->pos] == '\0');
}

static u8 Consume(jLexer* lexer) {
    lexer->curr = lexer->src[lexer->pos];
    if (lexer->curr == '\n') {
        lexer->line++;
        lexer->column = 0;
    } else {
        lexer->column++;
    }

    lexer->pos++;
    lexer->next = lexer->src[lexer->pos];
    return lexer->curr;
}

static void SkipComment(jLexer* lexer) {
    u8 end = (Consume(lexer) == '$') ? '\n' : '$';
    while (!IsEOF(lexer) && Consume(lexer) != end);
}

static void TokenizeCharacter(jLexer* lexer, jToken** tokens) {
    jToken token = {
        .line   = lexer->line,
        .column = lexer->token_start,
        .length = 1,
        .type   = jTryGetToken(&lexer->curr, 1),
    };

    if (token.type & JT_OP) { token.opinfo = _J_OPINFOS[token.type]; }
    J_ARRAY_PUSH(*tokens, token);
}

static void TokenizeNumericLiteral(jLexer* lexer, jToken** tokens) {
    b8  is_float        = false;
    b8  ignore_rest     = false;
    u8* numeric_literal = J_ARRAY_CREATE(u8);
    while (!IsEOF(lexer)) {
        if (isdigit(lexer->curr)) {
            if (!ignore_rest) { J_ARRAY_PUSH(numeric_literal, lexer->curr); }
        }

        else if (lexer->curr == '.') {
            if (is_float) {
                ignore_rest = true;
            } else {
                is_float = true;
                J_ARRAY_PUSH(numeric_literal, '.');
            }
        }

        if (isdigit(lexer->next) || lexer->next == '.' || lexer->next == '_') {
            Consume(lexer);
        } else {
            break;
        }
    }
    J_ARRAY_PUSH(numeric_literal, '\0');

    jToken token = {
        .data   = numeric_literal,
        .line   = lexer->line,
        .column = lexer->token_start,
        .length = J_ARRAY_SIZE(numeric_literal),
        .type   = is_float ? JT_LIT_FLOAT : JT_LIT_INT,
    };

    J_ARRAY_PUSH(*tokens, token);
}

static void TokenizeIdentifier(jLexer* lexer, jToken** tokens) {
    u8* identifier = J_ARRAY_CREATE(u8);
    while (!IsEOF(lexer) && (isalnum(lexer->curr) || lexer->curr == '_')) {
        J_ARRAY_PUSH(identifier, lexer->curr);
        if (isalnum(lexer->next) || lexer->next == '_') {
            Consume(lexer);
        } else {
            break;
        }
    }
    J_ARRAY_PUSH(identifier, '\0');

    enum j_token_t type  = jTryGetToken(identifier, strlen((const char*)identifier));
    jToken         token = {
                .data   = identifier,
                .line   = lexer->line,
                .column = lexer->token_start,
                .length = J_ARRAY_SIZE(identifier),
                .type   = type ? type : JT_ID,
    };

    J_ARRAY_PUSH(*tokens, token);
}

static void TokenizeStringLiteral(jLexer* lexer, jToken** tokens) {
    u8* string_literal = J_ARRAY_CREATE(u8);
    while (!IsEOF(lexer) && Consume(lexer) != '"') { J_ARRAY_PUSH(string_literal, lexer->curr); }
    J_ARRAY_PUSH(string_literal, '\0');

    jToken token = {
        .data   = string_literal,
        .line   = lexer->line,
        .column = lexer->token_start,
        .length = J_ARRAY_SIZE(string_literal),
        .type   = JT_LIT_STR,
    };

    J_ARRAY_PUSH(*tokens, token);
}

static void TokenizeEOF(jLexer* lexer, jToken** tokens) {
    jToken token = {
        .line   = lexer->line,
        .column = 0,
        .type   = JT_EOF,
    };

    J_ARRAY_PUSH(*tokens, token);
}

void jTokenize(jLexer* lexer, jToken** tokens) {
    jPrehashTokenStrings();

    while (!IsEOF(lexer)) {
        lexer->token_start = lexer->column;

        switch (Consume(lexer)) {

        case '$': SkipComment(lexer); break;
        case '"': TokenizeStringLiteral(lexer, tokens); break;

        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
        case '<':
        case '>':
        case '&':
        case '|':

        case '[':
        case ']':
        case '(':
        case ')':
        case '{':
        case '}':
        case '.':
        case ',':
        case ':':
        case ';': TokenizeCharacter(lexer, tokens); break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': TokenizeNumericLiteral(lexer, tokens); break;

        case '_':
        case 'A':
        case 'a':
        case 'B':
        case 'b':
        case 'C':
        case 'c':
        case 'D':
        case 'd':
        case 'E':
        case 'e':
        case 'F':
        case 'f':
        case 'G':
        case 'g':
        case 'H':
        case 'h':
        case 'I':
        case 'i':
        case 'J':
        case 'j':
        case 'K':
        case 'k':
        case 'L':
        case 'l':
        case 'M':
        case 'm':
        case 'N':
        case 'n':
        case 'O':
        case 'o':
        case 'P':
        case 'p':
        case 'Q':
        case 'q':
        case 'R':
        case 'r':
        case 'S':
        case 's':
        case 'T':
        case 't':
        case 'U':
        case 'u':
        case 'V':
        case 'v':
        case 'W':
        case 'w':
        case 'X':
        case 'x':
        case 'Y':
        case 'y':
        case 'Z':
        case 'z': TokenizeIdentifier(lexer, tokens); break;

        default: break;
        }
    }

    TokenizeEOF(lexer, tokens);
}
