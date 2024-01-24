#include "ast_redir.h"

#include "../ast.h"

void free_redir(struct ast_redir *ast)
{
    free_ast(ast->left);
    ast->left = NULL;

    free_ast(ast->right);
    ast->right = NULL;
}

void free_file(struct ast_file *ast)
{
    free(ast->filename);
}
