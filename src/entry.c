#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug_ast.h"
#include "error.h"
#include "nodes.h"
#include "parse.h"
#include "tokenize.h"
#include "tokens.h"
#include "types.h"

i32 main(i32 argc, char** argv) {

    if (argc < 2) {
        fputs("Type jacl --help (or -h) for help.\n", stderr);
        return 1;
    }

    if (!strncmp(argv[1], "--help", 6) || !strncmp(argv[1], "-h", 2)) {
        puts("JACL HELP:\n");
        puts("  [-s] source file(s)\n");
        puts("  [-o] object file name\n");
        puts("  [-i] output intermediate file(s)\n");
        return 0;
    }

    const char* filename = argv[1];

    char*       src      = NULL;
    u64         src_len  = 0;
    {
        FILE* file = fopen(filename, "rb");
        if (!file) {
            perror("Cannot open source file.");
            return 1;
        }

        fseek(file, 0, SEEK_END);
        src_len = ftell(file);
        fseek(file, 0, SEEK_SET);

        src = malloc((src_len + 1) * sizeof(char));
        if (!src) {
            fputs("Could not allocate memory for source buffer.", stderr);
            return 1;
        }

        fread(src, sizeof(char), src_len, file);
        if (ferror(file)) {
            fputs("Error occurred while reading source file.", stderr);
            return 1;
        }

        fclose(file);
        src[src_len++] = '\0';
    }

    enum j_error_t err;
    jToken         tokens[1024];
    u64            token_count = 0;
    if ((err = jTokenize(src, src_len, tokens, &token_count))) {
        fputs("Could not tokenize.\n", stderr);
        return 1;
    }

#if 0
    fputs("\nTOKEN STACK:\n", stderr);
    for (u64 i = 0; i < token_count; ++i) { jPrintDebugToken(&tokens[i]); }
#endif

    jNodeRoot program = { .stmts = malloc(1024 * sizeof(jNodeStmt)) };
    jParser   parser  = { .tokens = tokens, .token_count = token_count };
    if ((err = jParse(&parser, &program))) {
        fputs("Could not parse.\n", stderr);
        return 1;
    }

    _jDebugProgramTree(&program);
    return 0;
}
