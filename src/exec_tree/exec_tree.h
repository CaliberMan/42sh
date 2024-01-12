#ifndef EXEC_TREE_H
#define EXEC_TREE_H

#include "../ast/ast.h"
#include "../exec/exec.h"
#include "../builtins/builtins.h"
#include <string.h>

/**
 * @brief Executes the given tree
 * @param ast The ast going to be executed
 * @return Returns -1 if the execution failled, 1 if the output of the excecution is false, 0 if everything went well
 */
int execute_tree(struct ast *ast);
#endif /* ! EXEC_TREE_H */
