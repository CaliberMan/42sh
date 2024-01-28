#include "ast_list.h"
#include <stdlib.h>

#include "../ast.h"

void init_list(struct ast_list *ast)
{
    ast->list = calloc(5, sizeof(struct ast *));
    ast->capacity = 5;
}

void free_ast_list(struct ast_list *list)
{
    for (size_t i = 0; i < list->capacity; i++)
    {
        if (!list->list[i])
            free(list->list[i]);
        else
            free_ast(list->list[i]);
    }

    free(list);
}

void double_list_size(struct ast_list *ast)
{
    ast->list = realloc(ast->list, sizeof(struct ast *) * ast->capacity * 2);
    for (size_t i = ast->capacity; i < ast->capacity * 2; i++)
        ast->list[i] = NULL;

    ast->capacity *= 2;
}
