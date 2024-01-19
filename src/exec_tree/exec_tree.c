#include "exec_tree.h"

static int check_builtins(char **command)
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
    case AST_IF:;
        struct ast_if if_struct = ast->data.ast_if;
        ans = execute_tree(if_struct.cond);
        if (ans == 0)
            ans = execute_tree(if_struct.then_body);
        else if (ans == 1)
            ans = execute_tree(if_struct.else_body);
        else
            ans = -1;
        if (ast->next != NULL)
            execute_tree(ast->next);
        return ans;
        break;
    case AST_CMD:;
        struct ast_cmd cmd_struct = ast->data.ast_cmd;
        ans = check_builtins(cmd_struct.words);
        if (ast->next != NULL)
            execute_tree(ast->next);
        return ans;
        break;
    default:
        return -1;
        break;
    }
}
