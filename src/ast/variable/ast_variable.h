#ifndef AST_VARIABLE_H
#define AST_VARIABLE_H

struct ast_variable
{
    char *name;
    struct ast *value;
};

void free_variable(struct ast_variable *ast);

#endif /* ! AST_VARIABLE_H */
