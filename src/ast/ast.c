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
    else if (ast->type == AST_IF)
        free_if(&ast->data.ast_if);
    else if (ast->type == AST_PIPE)
        free_pipe(&ast->data.ast_pipe);
    else if (ast->type == AST_REDIR)
        free_redir(&ast->data.ast_redir);
    else if (ast->type == AST_FILE)
        free_file(&ast->data.ast_file);
    else if (ast->type == AST_LOOP)
        free_loop(&ast->data.ast_loop);
    else if (ast->type == AST_OPERATOR)
        free_operator(&ast->data.ast_operator);
    else if (ast->type == AST_NOT)
        free_not(&ast->data.ast_not);
    else if (ast->type == AST_VARIABLE)
        free_variable(&ast->data.ast_variable);

    if (ast->next)
    {
        free_ast(ast->next);
        ast->next = NULL;
    }

    free(ast);
}
