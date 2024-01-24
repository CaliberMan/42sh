#include "ast_not.h"

#include "../ast.h"

void free_not(struct ast_not *ast)
{
    free_ast(ast->child);
    ast->child = NULL;
}
