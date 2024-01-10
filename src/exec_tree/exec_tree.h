#include "../ast/ast.h"

/**
 * @brief Executes the given tree
 * @param ast The ast going to be executed
 * @return Returns -1 if the execution failled, 1 if the output of the excecution is false, 0 if everything went well
 */
int execute_tree(struct ast *ast);