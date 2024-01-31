#ifndef EXEC_TREE_H
#define EXEC_TREE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../ast/ast.h"
#include "../builtins/builtins.h"
#include "../exec/exec.h"
#include "./variables/variable.h"

/**
 * @brief Executes the given tree
 * @param ast The ast going to be executed
 * @return Returns -1 if the execution failled, 1 if the output of the
 * excecution is false, 0 if everything went well
 */
int execute_tree(struct ast *ast, struct exec_arguments args);

#endif /* ! EXEC_TREE_H */
