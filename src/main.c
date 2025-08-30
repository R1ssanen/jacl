#include <stdio.h>
#include <time.h>

#include "arena.h"
#include "ast.h"
#include "diagnostics.h"
// #include "gen.h"
// #include "ir.h"
#include "lex.h"

int main(int argc, char* argv[]) {
    clock_t time_start = clock();

    if (argc == 1) {
        fprintf(stderr, "jac: no files to compile, exiting.\n");
        return 0;
    }

    const char* source_path = argv[1];
    printf("jac: compiling %s...\n", source_path);

    char* source = NULL;
    {
        FILE* file = fopen(source_path, "r");
        if (!file) {
            fprintf(stderr, "error: could not open source file '%s', exiting.\n", source_path);
            return 1;
        }

        fseek(file, 0, SEEK_END);
        size_t length = ftell(file) + 1;
        rewind(file);

        source = malloc(length * sizeof(char));
        fread((void*)source, sizeof(char), length, file);
        fclose(file);
    }

    jac_token* tokens = jac_tokenize(source);
    if (!tokens) {
        free((void*)source);
        return 1;
    }

    jac_memory_arena arena = { .memory = darray_new_reserved(char, 1024 * 2) };
    jac_ast_unit     unit;
    if (!jac_parse_unit(&arena, tokens, &unit)) {
        jac_free_arena(&arena);
        jac_free_tokens(tokens);
        free((void*)source);
        return 1;
    }

    jac_print_unit(unit, 3);

    // darray_t char* asm_source = NULL;
    // if (!jac_generate_unit(&unit, &asm_source)) {
    //     jac_free_unit(&unit);
    //     jac_free_arena(&arena);
    //     jac_free_tokens(tokens);
    //     free((void*)source);
    //     return 1;
    // }

    // {
    //     const char* source_name = strrchr(source_path, '\\');
    //     if (!source_name) source_name = strrchr(source_path, '/');
    //     if (!source_name) source_name = source_path;
    //     else
    //         source_name += 1;

    //     const char* output_dir = (argc == 2) ? "." : argv[2];
    //     char        full_path[256];
    //     snprintf(full_path, 256, "%s/%s.s", output_dir, source_name);

    //     FILE* file = fopen(full_path, "w");
    //     if (!file) {
    //         darray_free(asm_source);
    //         jac_free_unit(&unit);
    //         jac_free_arena(&arena);
    //         jac_free_tokens(tokens);
    //         free((void*)source);

    //         fprintf(stderr, "error: could not open file '%s'.\n", full_path);
    //         return 1;
    //     }

    //     fwrite((void*)asm_source, sizeof(char), darray_count(asm_source), file);
    //     fclose(file);
    // }

    // darray_free(asm_source);
    jac_free_unit(unit);
    jac_free_arena(&arena);
    jac_free_tokens(tokens);
    free((void*)source);

    printf(
        "jac: compilation finished in %.4fs.\n", (double)(clock() - time_start) / CLOCKS_PER_SEC
    );
    return 0;
}
