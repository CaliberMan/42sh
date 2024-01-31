#ifndef AST_FUNC_H
#define AST_FUNC_H

struct ast_func
{
    char *name;
    struct ast *body;
};

void free_func(struct ast_func *ast);

#endif /* ! AST_FUNC_H */
