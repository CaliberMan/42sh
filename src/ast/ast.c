#include "ast.h"

#include <stdlib.h>

struct ast *init_ast(enum ast_type)
{
    struct ast *ast = calloc(1, sizeof(struct ast));
    if (!ast)
        return NULL;

    ast->command = calloc(5, sizeof(char *));
    ast->type = ast_type;
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

    if (ast->command)
        free(command);

    free(ast);
}
