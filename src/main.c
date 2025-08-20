#include <stdio.h>

#include "arena.h"
#include "ast.h"
#include "gen.h"
#include "lex.h"

static void free_resources(jac_token* tokens, jac_node_unit* unit, jac_memory_arena* arena) {
    jac_free_unit(unit);
    jac_free_tokens(tokens);
    jac_free_arena(arena);
}

int main() {

    char* source = NULL;
    {
        FILE* file = fopen("examples/1.j", "r");
        fseek(file, 0, SEEK_END);
        size_t len = ftell(file) + 1;
        rewind(file);

        source = malloc(len * sizeof(char));
        fread((void*)source, sizeof(char), len, file);
        fclose(file);
    }

    jac_token* tokens = jac_tokenize(source);
    if (!tokens) return 1;

    free((void*)source);

    jac_memory_arena arena = { .memory = darray_new_reserved(char, 1024 * 2) };
    jac_node_unit    unit;
    if (!jac_parse_unit(&arena, tokens, &unit)) {
        free_resources(tokens, &unit, &arena);
        return 1;
    }

    jac_print_unit(&unit, 3);

    darray_t char* asm_source = NULL;
    if (!jac_generate_unit(&unit, &asm_source)) {
        free_resources(tokens, &unit, &arena);
        return 1;
    }

    {
        FILE* file = fopen("examples/1.build/1.s", "w");
        fwrite((void*)asm_source, sizeof(char), darray_count(asm_source), file);
        fclose(file);
    }

    printf("\n*** ASM ***\n%s\n**********\n", asm_source);
    darray_free(asm_source);

    free_resources(tokens, &unit, &arena);
    return 0;
}
