#include "lex.h"

#include <ctype.h>
#include <stdbool.h>

#include "darray.h"

struct {
    const char*    name;
    jac_token_kind kind;
} keywords[] = {
    { "return", JAC_TOKEN_RETURN },
    { "func",   JAC_TOKEN_FUNC   },
    { "extern", JAC_TOKEN_EXTERN },
    { "i32",    JAC_TOKEN_INT32  },
    { "u8",     JAC_TOKEN_UINT8  },
};

// NOTE: expects null-termination
static bool is_keyword(darray_t char* value, jac_token_kind* kind) {

    for (size_t i = 0; i < (sizeof(keywords) / sizeof(*keywords)); ++i) {
        const char*    key_name = keywords[i].name;
        jac_token_kind key_kind = keywords[i].kind;

        if (strcmp(key_name, value) == 0) {
            *kind = key_kind;
            return true;
        }
    }

    return false;
}

typedef struct {
    const char* source;
    const char* c;
    size_t      line, col;
} lexer;

static char consume(lexer* lexer) {
    if (*lexer->c == '\n') {
        lexer->line += 1;
        lexer->col = 1;
    } else {
        lexer->col += 1;
    }

    return *(lexer->c++);
}

static jac_token new_token(darray_t char* value, jac_token_kind kind, const lexer* lexer) {
    return (jac_token){ .value = value,
                        .kind  = kind,
                        .pos   = lexer->c - lexer->source,
                        .line  = lexer->line,
                        .col   = lexer->col };
}

static jac_token new_token_from_char(char value, const lexer* lexer) {
    char* token_value = darray_new(char);
    darray_push(token_value, value);
    return new_token(token_value, (jac_token_kind)value, lexer);
}

darray_t jac_token* jac_tokenize(const char* source) {

    jac_token* tokens  = darray_new(jac_token);
    lexer      lexer   = { .source = source, .c = source, .line = 1, .col = 1 };
    bool       invalid = false;

    for (; *lexer.c != '\0'; consume(&lexer)) {

        if (isspace(*lexer.c)) {
            continue;
        }

        else if (lexer.c[0] == '-' && lexer.c[1] == '-') {
            while (*lexer.c != '\n') consume(&lexer);
        }

        else if (isalpha(*lexer.c) || (*lexer.c == '_')) {
            char* value = darray_new(char);
            darray_push(value, *lexer.c);

            while (isalnum(lexer.c[1]) || (lexer.c[1] == '_')) {
                consume(&lexer);
                darray_push(value, *lexer.c);
            }
            darray_push(value, '\0');

            jac_token_kind keyword_kind;
            if (is_keyword(value, &keyword_kind)) {
                // printf("keyword: %s (%lu, %lu)\n", value, lexer.line, lexer.col);
                darray_push(tokens, new_token(value, keyword_kind, &lexer));
            } else {
                // printf("identifier: %s (%lu, %lu)\n", value, lexer.line, lexer.col);
                darray_push(tokens, new_token(value, JAC_TOKEN_IDENTIFIER, &lexer));
            }
        }

        else if (isdigit(*lexer.c) || ((*lexer.c == '-') && isdigit(lexer.c[1]))) {
            char* value = darray_new(char);
            darray_push(value, *lexer.c);

            while (isdigit(lexer.c[1])) {
                consume(&lexer);
                darray_push(value, *lexer.c);
            }
            darray_push(value, '\0');

            // printf("literal: %s (%lu, %lu)\n", value, lexer.line, lexer.col);
            darray_push(tokens, new_token(value, JAC_TOKEN_LITERAL, &lexer));
        }

        else if (strchr(";:,{}()*->&", *lexer.c)) {
            // printf("delimiter: %c (%lu, %lu)\n", *lexer.c, lexer.line, lexer.col);
            darray_push(tokens, new_token_from_char(*lexer.c, &lexer));
        }

        else {
            fprintf(
                stderr, "error; unrecognized character '%c' at [%lu, %lu]\n", *lexer.c, lexer.line,
                lexer.col
            );
            invalid = true;
        }
    }

    if (invalid) {
        jac_free_tokens(tokens);
        return NULL;
    }

    darray_push(tokens, new_token_from_char('\0', &lexer));
    return tokens;
}

void jac_free_tokens(darray_t jac_token* tokens) {
    darray_foreach(tokens, jac_token, token) { darray_free(token->value); }
    darray_free(tokens);
}
