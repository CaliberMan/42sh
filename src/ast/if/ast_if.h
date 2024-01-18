#ifndef AST_IF_H
#define AST_IF_H

struct ast_if
{
    struct ast *cond; // condition
    struct ast *then_body; // if true
    struct ast *else_body; // if false
};

#endif /* ! AST_IF_H */
