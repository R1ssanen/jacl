#ifndef _ERROR_H_
#define _ERROR_H_

#include "types.h"

#define J_MAX_ERRORS 16

enum j_error_t {
    JE_SUCCESS = 0,
    JE_SYNTAX,
    JE_SEMANTIC,
};

typedef struct {
    const char*    msg;
    u64            msg_len;
    u64            line, col;
    enum j_error_t type;
} jError;

typedef struct {
    jError errors[J_MAX_ERRORS];
    u64    error_count;
} jErrorHandler;

void jRaiseError(jErrorHandler* err, enum j_error_t type, const char* msg, u64 line, u64 column);

void jPrintErrorStack(const jErrorHandler* err);

const char* jGetErrnoString(void);

#endif
