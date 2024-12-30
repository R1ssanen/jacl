#include "error.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

const char* _J_ERROR_STRINGS[] = {
    [JE_SYNTAX]   = "syntax error",
    [JE_SEMANTIC] = "semantic error",
};

void jRaiseError(jErrorHandler* err, enum j_error_t type, const char* msg, u64 line, u64 column) {
    err->errors[err->error_count++] = (jError){
        .msg     = msg,
        .msg_len = strlen(msg),
        .line    = line,
        .col     = column,
        .type    = type,
    };
}

void jPrintErrorStack(const jErrorHandler* err) {
    fputs("error stack bactrace:\n", stderr);

    for (u64 i = 0; i < err->error_count; ++i) {
        const jError* error = &err->errors[i];
        fprintf(
            stderr, "\t%s: %s [%lu, %lu]\n", _J_ERROR_STRINGS[error->type], error->msg, error->line,
            error->col
        );
    }
}

const char* jGetErrnoString(void) { return strerror(errno); }
