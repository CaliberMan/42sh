#include "ast_operator.h"

#include "../ast.h"

void free_operator(struct ast_operator *ast)
{
    free_ast(ast->left);
    ast->left = NULL;

    free_ast(ast->right);
    ast->right = NULL;
}
