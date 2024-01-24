#ifndef AST_LOOP_H
#define AST_LOOP_H

#include "../../exec_tree/variables/variable.h"

enum loop_type
{
    WHILE_LOOP,
    UNTIL_LOOP,
    FOR_LOOP
};

struct ast_loop
{
    enum loop_type type;
    struct variable *var;
    struct ast *cond;
    struct ast *then_body;
};

/**
 * @brief Freeing the ast loop
 * @param ast The ast to free
 */
void free_loop(struct ast_loop *ast);

#endif /* ! AST_LOOP */
