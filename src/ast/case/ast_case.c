#include "ast_case.h"

#include "../ast.h"

void free_case(struct ast_case *ast)
{
    free(ast->expr);
    ast->expr = NULL;

    free_ast(ast->cases_list);
    ast->cases_list = NULL;
}

void free_pattern(struct ast_pattern *ast)
{
    free_ast(ast->pattern);
    ast->pattern = NULL;

    free_ast(ast->statement);
    ast->statement = NULL;
}
