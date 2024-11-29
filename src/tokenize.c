#include "tokenize.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "tokens.h"
#include "types.h"

_jOpInfo _J_OPINFOS[] = {
    [JT_OP_OR]  = { J_ASSOC_LEFT, 0 },
    [JT_OP_AND] = { J_ASSOC_LEFT, 1 }

    ,
    [JT_OP_EQ] = { J_ASSOC_LEFT, 2 }

    ,
    [JT_OP_ADD] = { J_ASSOC_LEFT, 3 },
    [JT_OP_SUB] = { J_ASSOC_LEFT, 3 }

    ,
    [JT_OP_MUL] = { J_ASSOC_LEFT, 4 },
    [JT_OP_DIV] = { J_ASSOC_LEFT, 4 },
};

enum j_error_t jTokenize(const char* src, u64 src_len, jToken* tokens, u64* token_count) {

    jPrehashLongTokens();
    u64 line = 0;

    for (u64 i = 0; i < src_len; ++i) {
        enum j_token_t type = jTryGetShortToken(src[i]);

        if (src[i] == '\n') {
            ++line;
        }

        else if (src[i] == '$') {
            if (src[++i] == '$') { // single line
                while (src[++i] != '\n');

            } else { // multiline
                while (src[++i] != '$') {
                    if (src[i] == '\n') { ++line; }
                }
            }
        }

        else if (type & JT_OP) {
            tokens[(*token_count)++] = (jToken){
                .opinfo = _J_OPINFOS[type],
                .line   = line,
                .type   = type,
            };
        }

        else if (type & JT_SPEC) {
            tokens[(*token_count)++] = (jToken){
                .line = line,
                .type = type,
            };
        }

        else if (isalpha(src[i]) || (src[i] == '_')) {
            u64 start = i;
            while (isalnum(src[i + 1]) || (src[i + 1] == '_')) { ++i; }

            u64   len = i - start + 1;
            char* str = malloc((len + 1) * sizeof(char));
            strncpy(str, src + start, len);
            str[len]                 = '\0';

            type                     = jTryGetLongToken(str);
            tokens[(*token_count)++] = (jToken){
                .str  = str,
                .line = line,
                .type = type ? type : JT_ID,
            };
        }

        else if (isdigit(src[i])) {
            u64 start = i;
            while (isalnum(src[i + 1])) { ++i; }

            u64  len = i - start + 1;
            char data[len + 1];
            strncpy(data, src + start, len);
            data[len]                = '\0';

            tokens[(*token_count)++] = (jToken){
                .int_value = strtol(data, NULL, 10),
                .line      = line,
                .type      = JT_LIT_INT,
            };
        }

        else if (src[i] == '\"') {
            u64 start = ++i;
            while (src[i] != '\"') { ++i; }

            u64   len = i - start;
            char* str = malloc((len + 1) * sizeof(char));
            strncpy(str, src + start, len);
            str[len]                 = '\0';

            tokens[(*token_count)++] = (jToken){
                .str  = str,
                .line = line,
                .type = JT_LIT_STR,
            };
        }

        else {
        }
    }

    return J_SUCCESS;
}
