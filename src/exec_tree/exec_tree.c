#include "exec_tree.h"

int execute_tree(struct ast *ast)
{
    if (!ast)
        return 0;   
 
    switch (ast->type)
    {
    case AST_IF:
        //if (exec(ast->command))
        //   return execute_tree(ast->left);
        //else
        //{
        //  return execute_tree(ast->right);
        //}
        break;
    case AST_COMMAND:
        //return exec(ast->command);
        break;
    default:
        return -1;
        break;
    }
}