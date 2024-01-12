#include "exec_tree.h"

int execute_tree(struct ast *ast)
{
    if (!ast)
        return 0;
    int ans;
    switch (ast->type)
    {
    case AST_IF:
        ans = execute_tree(ast->op_ast);
        if (ans == 0)
            ans = execute_tree(ast->left);
        else if (ans == 1)
            ans = execute_tree(ast->right);
        else
            ans = -1;
        if (ast->next != NULL)
            execute_tree(ast->next);
        return ans;
        break;
    case AST_COMMAND:
        ans = exec(ast->command);
        if (ast->next != NULL)
            execute_tree(ast->next);
        return ans;
        break;
    default:
        return -1;
        break;
    }
}
