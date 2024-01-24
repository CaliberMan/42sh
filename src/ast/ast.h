#ifndef AST_H
#define AST_H

#include "cmd/ast_cmd.h"
#include "if/ast_if.h"
#include "pipe/ast_pipe.h"
#include "redir/ast_redir.h"
#include "loop/ast_loop.h"

enum ast_type
{
    AST_IF,
    AST_CMD,
    AST_PIPE,
    AST_REDIR,
    AST_LOOP
};

union ast_union
{
    struct ast_if ast_if; // node of type ast_if
    struct ast_cmd ast_cmd; // node of type ast_cmd
    struct ast_pipe ast_pipe; // node of type ast_pipe
    struct ast_redir ast_redir; // node of type ast_redir
    struct ast_loop ast_loop; // node of type ast_loop
};

struct ast
{
    enum ast_type type; // type of node
    union ast_union data; // the union of all types of nodes
    struct ast *next; // the next ast
};

/**
 * @brief Frees an ast recursively
 * @param ast The ast to free
 */
void free_ast(struct ast *ast);

/**
 * @brief Creates an empty ast
 * @return The empty ast
 */
struct ast *init_ast(enum ast_type type);

#endif /* ! AST_H */
