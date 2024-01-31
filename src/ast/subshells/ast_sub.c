#include "ast_sub.h"

#include "../ast.h"

void free_sub(struct ast_sub *ast)
{
    free_ast(ast->list);
    ast->list = NULL;
}
