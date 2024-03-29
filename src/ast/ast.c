#include "ast.h"

#include <stdlib.h>

struct ast *init_ast(enum ast_type type)
{
    struct ast *ast = calloc(1, sizeof(struct ast));
    ast->type = type;

    if (type == AST_CMD)
        init_words(&ast->data.ast_cmd);
    else if (type == AST_LIST)
        init_list(&ast->data.ast_list);

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
    else if (ast->type == AST_LIST)
        free_ast_list(&ast->data.ast_list);
    else if (ast->type == AST_FUNCTION)
        free_func(&ast->data.ast_func);
    else if (ast->type == AST_SUBSHELL)
        free_sub(&ast->data.ast_sub);
    else if (ast->type == AST_CASE)
        free_case(&ast->data.ast_case);
    else if (ast->type == AST_PATTERN)
        free_pattern(&ast->data.ast_pattern);

    free(ast);
}
