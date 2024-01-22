#include "ast.h"

#include <stdlib.h>

struct ast *init_ast(enum ast_type type)
{
    struct ast *ast = calloc(1, sizeof(struct ast));
    ast->type = type;
    return ast;
}

void free_ast(struct ast *ast)
{
    if (!ast)
        return;

    if (ast->type == AST_CMD)
        free_cmd(&ast->data.ast_cmd);
    if (ast->type == AST_IF)
        free_if(&ast->data.ast_if);
    if (ast->type == AST_PIPE)
        free_pipe(&ast->data.ast_pipe);
    if (ast->type == AST_REDIR)
        free_redir(&ast->data.ast_redir);
        
    if (ast->next)
    {
        free_ast(ast->next);
        ast->next = NULL;
    }

    free(ast);
}
