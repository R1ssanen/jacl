#include <jaclang.h>
#include <stdlib.h>
#include <string.h>

i32 main(i32 argc, char** argv) {
    if (argc < 2) {
        J_FATAL("Source file needed.\n");
        return 1;
    }

    const char* filename = argv[1];
    u8*         src      = NULL;
    u64         src_len  = 0;
    {
        FILE* file = fopen(filename, "rb");
        if (!file) {
            J_FATAL("Cannot open source file.\n %s\n", jGetErrnoString());
            return 1;
        }

        fseek(file, 0, SEEK_END);
        src_len = ftell(file);
        fseek(file, 0, SEEK_SET);
        if (!src_len) {
            J_FATAL("Empty source file.\n");
            return 1;
        }

        src = malloc((src_len + 1) * sizeof(u8));
        if (!src) {
            free(src);
            J_FATAL("Could not allocate memory for source buffer.\n %s\n", jGetErrnoString());
            return 1;
        }

        fread((void*)src, sizeof(u8), src_len, file);
        if (ferror(file)) {
            free(src);
            J_FATAL("Error occurred while reading source file.\n %s\n", jGetErrnoString());
            return 1;
        }

        fclose(file);
        src[src_len++] = '\0';
    }

    jLexer  lexer  = { .src = src, .src_len = src_len, .line = 1, .column = 1 };
    jToken* tokens = J_ARRAY_INIT(jToken, 256);
    jTokenize(&lexer, &tokens);

#if 1
    fputs("\nTOKEN STACK:\n", stderr);
    for (u64 i = 0; i < J_ARRAY_SIZE(tokens); ++i) { jPrintDebugToken(&tokens[i]); }
    fputs("\n", stderr);
#endif

    jParser         parser     = { .tokens = tokens, .vars = J_ARRAY_CREATE(jNodeStatInit) };
    jError*         errors     = J_ARRAY_CREATE(jError);
    jNodeStatement* statements = J_ARRAY_INIT(jNodeStatement, 128);
    if (!jParse(&parser, &statements, &errors)) {
        jPrintErrorStack(errors);

        J_ARRAY_DESTROY(errors);
        J_ARRAY_DESTROY(statements);
        J_ARRAY_DESTROY(tokens);
        J_ARRAY_DESTROY(parser.vars);
        free(src);

        return 1;
    }

#if 0
    _jDebugProgramTree(statements);
#endif

    J_ARRAY_DESTROY(errors);
    J_ARRAY_DESTROY(statements);
    J_ARRAY_DESTROY(tokens);
    J_ARRAY_DESTROY(parser.vars);
    free(src);

    return 0;
}
