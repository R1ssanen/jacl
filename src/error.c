#include "error.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "array.h"

const char* _J_ERROR_STRINGS[] = {
    [JE_SYNTAX]   = "syntax error",
    [JE_SEMANTIC] = "semantic error",
};

void jPrintErrorStack(jError* err) {
    fputs("ERROR STACK BACKTRACE:\n", stderr);

    for (u64 i = 0; i < J_ARRAY_SIZE(err); ++i) {
        jError* error = err + i;
        fprintf(
            stderr, "\t%s: %s [%lu, %lu]\n", _J_ERROR_STRINGS[error->type], error->msg, 0,
            0 // error->token->line, error->token->column
        );
    }
}

const char* jGetErrnoString(void) { return strerror(errno); }
