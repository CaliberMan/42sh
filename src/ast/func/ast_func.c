#include "ast_func.h"

#include "../ast.h"

void free_func(struct ast_func *ast)
{
    if (ast->name)
        free(ast->name);

    free_ast(ast->body);
    ast->body = NULL;
}
