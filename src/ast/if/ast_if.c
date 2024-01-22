#include "ast_if.h"

#include "../ast.h"

void free_if(struct ast_if *ast)
{
    if (!ast)
        return;

    free_ast(ast->cond);
    ast->cond = NULL;

    free_ast(ast->then_body);
    ast->then_body = NULL;

    free_ast(ast->else_body);
    ast->else_body = NULL;
}
