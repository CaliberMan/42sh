#ifndef AST_H
#define AST_H

#include "cmd/ast_cmd.h"
#include "if/ast_if.h"

enum ast_type
{
    AST_IF,
    AST_CMD
};

union ast_union
{
    struct ast_if ast_if; // node of type ast_if
    struct ast_cmd ast_cmd; // node of type ast_cmd
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
struct ast *init_ast(void);

#endif /* ! AST_H */
