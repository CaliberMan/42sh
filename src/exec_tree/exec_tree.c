#include "exec_tree.h"

static int check_builtins(struct exec_arguments command)
{
    if (strcmp(command.args[0], "echo") == 0)
        return b_echo(command);
    if (strcmp(command.args[0], "true") == 0)
        return b_true();
    else if (strcmp(command.args[0], "false") == 0)
        return b_false();
    else
        return exec(command);
}

static int exec_cmd(struct exec_arguments describer, struct ast *ast)
{
    int ans;
    struct ast_cmd cmd_struct = ast->data.ast_cmd;
    describer.args = cmd_struct.words;
    ans = check_builtins(describer);
    if (ast->next != NULL)
        return execute_tree(ast->next, describer);
    return ans;
}

static int exec_if(struct exec_arguments describer, struct ast *ast)
{
    int ans;
    struct ast_if if_struct = ast->data.ast_if;
    ans = execute_tree(if_struct.cond, describer);
    if (ans == 0)
        ans = execute_tree(if_struct.then_body, describer);
    else if (ans == 1)
        ans = execute_tree(if_struct.else_body, describer);
    else
        ans = -1;
    if (ast->next != NULL)
        return execute_tree(ast->next, describer);
    return ans;
}

static int exec_pipe(struct exec_arguments describer, struct ast *ast)
{
    // Set up pipe
    int p[2];
    if (pipe(p) == -1)
        errx(1, "%s", "Bad pipe");

    // copy the contents of pipe into the struct
    memcpy(describer.pipe_fds, p, sizeof(p));
    // set up forking
    int fork_fd = fork();
    if (fork_fd < 0)
        errx(1, "%s\n", "Bad fork");

    int ans;
    describer.child_process = fork_fd;
    if (fork_fd == 0)
    {
        close(describer.pipe_fds[0]);
        dup2(describer.pipe_fds[1], STDOUT_FILENO);
        ans = execute_tree(ast->data.ast_pipe.left_arg, describer);
    }
    else
    {
        int status;
        waitpid(fork_fd, &status, 0);
        if (WIFEXITED(status))
        {
            int ex_st = WEXITSTATUS(status);
            if (ex_st == 127)
                return -1;
            else if (ex_st == 1)
                return 1;
            else
            {
                dup2(describer.pipe_fds[0], STDIN_FILENO);
                close(describer.pipe_fds[1]);
                ans = execute_tree(ast->data.ast_pipe.right_arg, describer);
            }
        }
    }
    return ans;
}

int execute_tree(struct ast *ast, struct exec_arguments describer)
{
    if (!ast)
        return 0;
    switch (ast->type)
    {
    case AST_IF:;
        return exec_if(describer, ast);
        break;
    case AST_CMD:;
        return exec_cmd(describer, ast);
        break;
    case AST_PIPE:;
        return exec_pipe(describer, ast);
        break;
    default:
        return -1;
        break;
    }
    return -1;
}
