#ifndef AST_REDIR_H
#define AST_REDIR_H

#include <stddef.h>

enum redir_type
{
    STD_IN,
    STD_OUT,
    STD_IN_OUT,
    STD_ERR,
    STD_OUT_END
};

struct ast_redir
{
    enum redir_type type;
    size_t ioNumber;
    struct ast *left;
    struct ast *right;
};

struct sfile
{
    char *filename;
};

void free_redir(struct ast_redir *ast);

#endif /* ! AST_REDIR_H */
