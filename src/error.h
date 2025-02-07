#ifndef _ERROR_H_
#define _ERROR_H_

#include "tokens.h"

#define J_MAX_ERRORS 4

enum j_error_t {
    JE_SUCCESS = 0,
    JE_SYNTAX,
    JE_SEMANTIC,
};

typedef struct {
    jToken*        token;
    const char*    msg;
    enum j_error_t type;
} jError;

#define J_NEW_ERROR(type, msg, token)                                                              \
    (jError) { token, msg, type }

void        jPrintErrorStack(jError* err);

const char* jGetErrnoString(void);

#endif
