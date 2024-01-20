#include "ast.h"

#include <stdlib.h>
#include "if/ast_if.h"

struct ast *init_ast(void)
{
    struct ast *ast = calloc(1, sizeof(struct ast));
    return ast;
}

void free_if_ast(struct ast_if *ast)
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

void free_ast(struct ast *ast)
{
    if (!ast)
        return;

    if (ast->type == AST_CMD)
        free_words(&ast->data.ast_cmd);
    if (ast->type == AST_IF)
        free_if_ast(&ast->data.ast_if);

    if (ast->next)
    {
        free_ast(ast->next);
        ast->next = NULL;
    }

    free(ast);
}
