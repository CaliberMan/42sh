#ifndef AST_SUB_H
#define AST_SUB_H

struct ast_sub
{
    struct ast *list;
};

void free_sub(struct ast_sub *ast);

#endif /* ! AST_SUB_H */
