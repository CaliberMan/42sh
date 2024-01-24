#ifndef AST_NOT_H
#define AST_NOT_H

struct ast_not
{
    struct ast *child;
};

void free_not(struct ast_not *ast);

#endif /* ! AST_NOT_H */
