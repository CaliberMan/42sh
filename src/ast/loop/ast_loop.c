#include "ast_loop.h"

#include "../ast.h"

void free_loop(struct ast_loop *ast)
{
    free_ast(ast->cond);
    ast->cond = NULL;

    free_ast(ast->then_body);
    ast->then_body = NULL;

    if (ast->type == FOR_LOOP)
        free(ast->var_name);
}
