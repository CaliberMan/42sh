#ifndef AST_OPERATOR_H
#define AST_OPERATOR_H

enum operator_type
{
    OP_OR,
    OP_AND
};

struct ast_operator
{
    enum operator_type type;
    struct ast *left;
    struct ast *right;
};

void free_operator(struct ast_operator *ast);

#endif /* ! AST_OPERATOR_H */
