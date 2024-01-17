#include "ast.h"

#include <stdlib.h>

struct ast *init_ast(enum ast_type type)
{
    struct ast *ast = calloc(1, sizeof(struct ast));
    if (!ast)
        return NULL;
    ast->command = calloc(10, sizeof(char *));
    ast->type = type;
    ast->capacity = 10;
    return ast;
}

void free_ast(struct ast *ast)
{
    if (!ast)
        return;

    free_ast(ast->left);
    ast->left = NULL;

    free_ast(ast->right);
    ast->right = NULL;

    free_ast(ast->op_ast);
    ast->op_ast = NULL;

    free_ast(ast->next);
    ast->next = NULL;

    size_t i = 0;
    while (i < ast->capacity)
        free(ast->command[i++]);

    free(ast->command);
    free(ast);
}
