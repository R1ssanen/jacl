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

typedef struct lexer {
    const char* source;
    const char* c;
    size_t      line, column;
    size_t      line_offset;
} lexer;

static char consume(lexer* lexer) {
    if (*lexer->c == '\n') {
        lexer->line += 1;
        lexer->line_offset += lexer->column;
        lexer->column = 1;
    } else {
        lexer->column += 1;
    }

    return *(lexer->c++);
}

static jac_token
new_token(darray_t char* value, jac_token_kind kind, size_t column, const lexer* lexer) {
    return (jac_token){
        .diagnostics = (jac_token_diagnostics){ .source_line = lexer->source + lexer->line_offset,
                                               .length      = strlen(value),
                                               .line        = lexer->line,
                                               .column      = column },
        .value       = value,
        .kind        = kind
    };
}

static jac_token new_token_from_char(char value, size_t column, const lexer* lexer) {
    char* token_value = darray_new(char);
    darray_push(token_value, value);
    darray_push(token_value, '\0');
    return new_token(token_value, (jac_token_kind)value, column, lexer);
}

darray_t jac_token* jac_tokenize(const char* source) {

    jac_token* tokens = darray_new(jac_token);
    lexer      lexer  = { .source = source, .c = source, .line = 1, .column = 1, .line_offset = 0 };
    size_t     column = 1;
    bool       invalid = false;

    for (; *lexer.c != '\0'; consume(&lexer), column = lexer.column) {

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
            if (is_keyword(value, &keyword_kind))
                darray_push(tokens, new_token(value, keyword_kind, column, &lexer));
            else
                darray_push(tokens, new_token(value, JAC_TOKEN_IDENTIFIER, column, &lexer));
        }

        else if (isdigit(*lexer.c) || ((*lexer.c == '-') && isdigit(lexer.c[1]))) {
            char* value = darray_new(char);
            darray_push(value, *lexer.c);

            while (isdigit(lexer.c[1])) {
                consume(&lexer);
                darray_push(value, *lexer.c);
            }
            darray_push(value, '\0');
            darray_push(tokens, new_token(value, JAC_TOKEN_INT_LITERAL, column, &lexer));
        }

        else if (*lexer.c == '"') {
            consume(&lexer);
            column += 1;
            char* value = darray_new(char);

            while (*lexer.c != '"') {
                char c = consume(&lexer);
                if (c != '\\') {
                    darray_push(value, c);
                    continue;
                }

                c = consume(&lexer);
                switch (c) {
                case 'n': darray_push(value, '\n'); break;
                case 't': darray_push(value, '\t'); break;
                case 'r': darray_push(value, '\r'); break;
                case 'b': darray_push(value, '\b'); break;
                case 'f': darray_push(value, '\f'); break;
                case 'v': darray_push(value, '\v'); break;
                case '0': darray_push(value, '\0'); break;

                default:
                    fprintf(
                        stderr, "error: unrecognized character '%c' at [%lu, %lu]\n", *lexer.c,
                        lexer.line, column
                    );
                    invalid = true;
                }
            }

            darray_push(value, '\0');
            darray_push(tokens, new_token(value, JAC_TOKEN_STR_LITERAL, column, &lexer));
        }

        else if (strchr(";:,{}()*->&=", *lexer.c)) {
            darray_push(tokens, new_token_from_char(*lexer.c, column, &lexer));
        }

        else {
            fprintf(
                stderr, "error: unrecognized character '%c' at [%lu, %lu]\n", *lexer.c, lexer.line,
                column
            );
            invalid = true;
        }
    }

    if (invalid) {
        jac_free_tokens(tokens);
        return NULL;
    }

    darray_push(tokens, new_token_from_char('\0', column, &lexer));
    return tokens;
}

void jac_free_tokens(darray_t jac_token* tokens) {
    darray_foreach(tokens, jac_token, token) { darray_free(token->value); }
    darray_free(tokens);
}
