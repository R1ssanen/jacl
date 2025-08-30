#include "symbol.h"

#include "darray.h"
#include "lex.h"

static char type_coding[] = {
    [JAC_TYPE_INT8] = 'c',   [JAC_TYPE_UINT8] = 'C', [JAC_TYPE_INT32] = 'i',
    [JAC_TYPE_UINT32] = 'I', [JAC_TYPE_INT64] = 'l', [JAC_TYPE_UINT64] = 'L',
};

const char* jac_mangle_function_name(const jac_token* identifier, darray_t jac_symbol* args) {
    char  mangled[1024];
    char* end = mangled;
    end[0]    = '_';
    end[1]    = 'N';
    end += 2;

    end += snprintf(end, 128, "%lu%s", identifier->diagnostics.length, identifier->value);

    darray_foreach(args, jac_symbol, arg) {
        if (arg->type.kind == JAC_TYPE_NONE) continue;

        for (size_t i = 0; i < arg->type.indirection; ++i) {
            *end = 'P';
            end += 1;
        }

        if (arg->type.kind == JAC_TYPE_CUSTOM) {
            end += snprintf(
                end, 128, "%lu%s", arg->type.token->diagnostics.length, arg->type.token->value
            );
        } else {
            *end = type_coding[arg->type.kind];
            end += 1;
        }
    }

    *end = '\0';
    end += 1;

    char* out = malloc(end - mangled);
    strcpy(out, mangled);
    return out;
}
