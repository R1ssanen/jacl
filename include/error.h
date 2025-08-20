#ifndef JAC_ERROR_H_
#define JAC_ERROR_H_

#include "lex.h"

typedef enum jac_error_kind {
    JAC_ERROR_INVALID,
    JAC_ERROR_SYNTAX,
} jac_error_kind;

typedef struct jac_error {
    union {
        const jac_token* token;
    } opt;

    const char*    msg;
    jac_error_kind kind;
} jac_error;

#define JAC_SET_SYNTAX_ERROR(error_ptr_, token_, msg_)                                             \
    do {                                                                                           \
        if (error_ptr_)                                                                            \
            *(error_ptr_) =                                                                        \
                (jac_error){ .opt = { .token = token_ }, .msg = msg_, .kind = JAC_ERROR_SYNTAX };  \
    } while (0)

#endif
