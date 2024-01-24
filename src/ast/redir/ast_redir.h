#ifndef AST_REDIR_H
#define AST_REDIR_H

#include <stddef.h>

enum redir_type
{
    STD_IN,
    STD_OUT,
    STD_IN_OUT,
    STD_ERR,
    STD_OUT_END,
    STD_RIGHT_ARROW_PIPE,
    STD_LEFT_ARROW_AND
};

struct ast_redir
{
    enum redir_type type;
    int ioNumber;
    struct ast *left;
    struct ast *right;
};

struct ast_file
{
    char *filename;
};

void free_redir(struct ast_redir *ast);

void free_file(struct ast_file *ast);

#endif /* ! AST_REDIR_H */
