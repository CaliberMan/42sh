#ifndef AST_REDIR_H
#define AST_REDIR_H

struct ast_redir
{
    char *type;
    char **file;
    struct ast *left_arg;
};

#endif /* ! AST_REDIR_H */
