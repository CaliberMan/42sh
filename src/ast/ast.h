#ifndef AST_H
#define AST_H

#include <stdlib.h>

#include "case/ast_case.h"
#include "cmd/ast_cmd.h"
#include "func/ast_func.h"
#include "if/ast_if.h"
#include "list/ast_list.h"
#include "loop/ast_loop.h"
#include "not/ast_not.h"
#include "operator/ast_operator.h"
#include "pipe/ast_pipe.h"
#include "redir/ast_redir.h"
#include "subshells/ast_sub.h"
#include "variable/ast_variable.h"

enum ast_type
{
    AST_IF,
    AST_CMD,
    AST_PIPE,
    AST_REDIR,
    AST_LOOP,
    AST_FILE,
    AST_OPERATOR,
    AST_NOT,
    AST_VARIABLE,
    AST_LIST,
    AST_FUNCTION,
    AST_SUBSHELL,
    AST_CASE,
    AST_PATTERN
};

union ast_union
{
    struct ast_if ast_if;
    struct ast_cmd ast_cmd;
    struct ast_pipe ast_pipe;
    struct ast_redir ast_redir;
    struct ast_file ast_file;
    struct ast_loop ast_loop;
    struct ast_operator ast_operator;
    struct ast_not ast_not;
    struct ast_variable ast_variable;
    struct ast_list ast_list;
    struct ast_func ast_func;
    struct ast_sub ast_sub;
    struct ast_case ast_case;
    struct ast_pattern ast_pattern;
};

struct ast
{
    enum ast_type type; // type of node
    union ast_union data; // the union of all types of nodes
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
