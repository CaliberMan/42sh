#include "ast_cmd.h"

#include <stdlib.h>

void init_words(struct ast_cmd *ast)
{
    ast->words = calloc(5, sizeof(char *));
    ast->capacity = 5;
}

void realloc_words(struct ast_cmd *ast)
{
    ast->words = realloc(ast->words, 2 * ast->capacity * sizeof(char *));
    for (size_t i = ast->capacity; i < 2 * ast->capacity; i++)
        ast->words[i] = NULL;

    ast->capacity *= 2;
}

void free_cmd(struct ast_cmd *ast)
{
    size_t capacity = ast->capacity;
    for (size_t i = 0; i < capacity; i++)
        free(ast->words[i]);

    free(ast->words);
}
