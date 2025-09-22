#include "diagnostics.h"

#include <stdarg.h>
#include <stdio.h>

#include "lex.h"

void output_diagnostic(FILE *stream, const jac_token *token, const char *fmt, va_list args)
{
    fprintf(stream, "\x1b[31;1merror:\x1b[0m ");
    vfprintf(stream, fmt, args);

    jac_token_diagnostics d = token->diagnostics;
    int line_digits = snprintf(NULL, 0, "%lu", d.line);
    fprintf(stream, "\n %*c |", line_digits, ' ');
    fprintf(stream, "\n \x1b[1m%lu\x1b[0m |    ", d.line);

    for (size_t i = 0; strchr("\n\0", d.source_line[i]) == NULL; ++i)
        fputc(d.source_line[i], stream);

    fprintf(stream, "\n %*c |   %*c", line_digits, ' ', (int)d.column, ' ');

    fprintf(stream, "\x1b[33m");
    for (size_t i = 0; i < d.length; ++i)
        fputc('^', stream);
    fprintf(stream, "\x1b[0m");

    fputc('\n', stream);
}

void jac_print_diagnostic(const jac_token *token, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    output_diagnostic(stderr, token, fmt, args);
    va_end(args);
}

void jac_log_diagnostic(FILE *file, const jac_token *token, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    output_diagnostic(file, token, fmt, args);
    va_end(args);
}
