#ifndef JAC_DIAGNOSTICS_H_
#define JAC_DIAGNOSTICS_H_

#include <stdio.h>

struct jac_token;

void jac_print_diagnostic(const struct jac_token* token, const char* fmt, ...);

void jac_log_diagnostic(FILE* file, const struct jac_token* token, const char* fmt, ...);

#endif
