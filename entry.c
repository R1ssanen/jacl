#include <jaclang.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"

i32 main(i32 argc, char** argv) {
    if (argc < 2) {
        fputs("jacl: source file needed.\n", stderr);
        return 1;
    }

    jArenaMemory mem;
    jInitMemArena(1024 * 1024 * 5, &mem);

    const char* filename = argv[1];
    char*       src      = NULL;
    u64         src_len  = 0;
    {
        FILE* file = fopen(filename, "rb");
        if (!file) {
            fprintf(stderr, "jacl: cannot open source file.\n %s\n", jGetErrnoString());
            goto fail;
        }

        fseek(file, 0, SEEK_END);
        src_len = ftell(file);
        fseek(file, 0, SEEK_SET);

        src = J_ALLOC_N(char, src_len + 1, &mem);
        fread(src, sizeof(char), src_len, file);
        if (ferror(file)) {
            fprintf(
                stderr, "jacl: error occurred while reading source file.\n %s\n", jGetErrnoString()
            );
            goto fail;
        }

        fclose(file);
        src[src_len++] = '\0';
    }

    jLexer  lexer  = { .src = src, .src_len = src_len, .line = 1, .col = 1 };
    jToken* tokens = J_ARRAY_CREATE(jToken);
    jTokenize(&lexer, tokens);

#if 1
    fputs("\nTOKEN STACK:\n", stderr);
    for (u64 i = 0; i < J_ARRAY_SIZE(tokens); ++i) { jPrintDebugToken(&tokens[i]); }
    fputs("\n", stderr);
#endif

    jErrorHandler err    = { .error_count = 0 };
    jParser       parser = { .tokens = tokens };
    jNodeStmt*    stmts  = J_ARRAY_CREATE(jNodeStmt);
    if (!jParse(&parser, stmts, &err)) {
        jPrintErrorStack(&err);
        goto fail;
    }

#if 0
    _jDebugProgramTree(&program);
#endif

    J_ARRAY_DESTROY(tokens);
    J_ARRAY_DESTROY(stmts);
    return 0;

fail:
    free(mem.block);
    return 1;
}
