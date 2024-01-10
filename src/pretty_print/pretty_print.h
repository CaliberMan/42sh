#include <stddef.h>
#include "../ast/ast.h"
#include <stdio.h>

/**
 * @brief Prints the AST tree
 * @param ast The ast tree that is about to be printed
 * @return 0 if it works 1 if it failled
 */
int pretty_print(struct ast *ast);