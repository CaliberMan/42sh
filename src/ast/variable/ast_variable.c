#include "ast_variable.h"

#include "../ast.h"

void free_variable(struct ast_variable *ast)
{
    if (ast->name)
    {
        free(ast->name);
        ast->name = NULL;
    }

    free_ast(ast->value);
    ast->value = NULL;
}
