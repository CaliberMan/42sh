#include "exec_tree.h"
#include <string.h>


int check_builtins(char **command)
{
    if (strcmp(command[0], "echo") == 0)
        return b_echo(command);
    else if (strcmp(command[0], "true") == 0)
        return b_true();
    else if (strcmp(command[0], "false") == 0)
        return b_false();
    else
        return exec(command);
}


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
        ans = check_builtins(ast->command);
        if (ast->next != NULL)
            execute_tree(ast->next);
        return ans;
        break;
    default:
        return -1;
        break;
    }
}


