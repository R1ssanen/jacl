#ifndef JAC_TYPE_H_
#define JAC_TYPE_H_

#include <stdint.h>

enum jac_type_kind
{
    JAC_TYPE_NONE,
    JAC_TYPE_CUSTOM,
    JAC_TYPE_INT8,
    JAC_TYPE_UINT8,
    JAC_TYPE_INT32,
    JAC_TYPE_UINT32,
    JAC_TYPE_INT64,
    JAC_TYPE_UINT64
};

typedef struct jac_type jac_type;

struct jac_type
{
    const struct jac_token *token;
    enum jac_type_kind kind;
    uint8_t indirection;
};

#endif
