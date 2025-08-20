#ifndef JAC_ASSERT_H_
#define JAC_ASSERT_H_

#include <stdio.h>
#include <stdlib.h>

#define JAC_ASSERT_FMT(expr, fmt, ...)                                                             \
    if (!(expr)) {                                                                                 \
        fprintf(                                                                                   \
            stderr, "jac assertion [" #expr "] failed at %s, line %d\n\t", __func__, __LINE__      \
        );                                                                                         \
        fprintf(stderr, fmt "\n", __VA_ARGS__);                                                    \
        abort();                                                                                   \
    }

#define JAC_ASSERT(expr, msg)                                                                      \
    if (!(expr)) {                                                                                 \
        fprintf(                                                                                   \
            stderr, "jac assertion [" #expr "] failed at %s, line %d\n\t", __func__, __LINE__      \
        );                                                                                         \
        fprintf(stderr, msg "\n");                                                                 \
        abort();                                                                                   \
    }

#endif
