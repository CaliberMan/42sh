#include "ast_pipe.h"

#include "../ast.h"

void free_pipe(struct ast_pipe *ast)
{
    free_ast(ast->left_arg);
    ast->left_arg = NULL;

    free_ast(ast->right_arg);
    ast->right_arg = NULL;
}
