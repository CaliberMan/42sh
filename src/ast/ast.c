#include "ast.h"

#include <stdlib.h>

struct ast *init_ast(void)
{
    struct ast *ast = calloc(1, sizeof(struct ast));
    return ast;
}

void free_ast(struct ast *ast)
{
    if (!ast)
        return;

    if (ast->type == AST_CMD)
        free_words(&ast->data.ast_cmd);

    if (!ast->next)
        return;

    free_ast(ast->next);
    ast->next = NULL;
}
