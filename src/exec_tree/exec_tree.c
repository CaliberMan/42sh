#include "exec_tree.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

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
    ans = variable_expansion(describer);
    if (ans != 0)
        return 1;
    ans = check_builtins(describer);
    return ans;
}

static int exec_if(struct exec_arguments describer, struct ast *ast)
{
    int ans;
    struct ast_if if_struct = ast->data.ast_if;
    ans = execute_tree(if_struct.cond, describer);
    if (ans == 0)
        ans = execute_tree(if_struct.then_body, describer);
    else if (if_struct.else_body != NULL)
        ans = execute_tree(if_struct.else_body, describer);
    else
        ans = 0;
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

    int ans = 0;
    describer.child_process = fork_fd;
    if (fork_fd == 0)
    {
        close(describer.pipe_fds[0]);
        dup2(describer.pipe_fds[1], STDOUT_FILENO);
        execute_tree(ast->data.ast_pipe.left_arg, describer);
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

static int wrong_file(char *name)
{
    fprintf(stderr, "%s: No such file or directory", name);
    return 1;
}

static int exec_redir(struct exec_arguments describer, struct ast *ast)
{
    int in_fd;
    int out_fd;
    int ionumber = ast->data.ast_redir.ioNumber;
    if (ast->type != AST_REDIR)
        return -1;
    switch(ast->data.ast_redir.type)
    {
    case STD_OUT:
    case STD_RIGHT_ARROW_PIPE:
        in_fd = ionumber != -1 ? ionumber : STDOUT_FILENO;
        out_fd = open(ast->data.ast_redir.right->data.ast_file.filename, O_CREAT | O_TRUNC | O_WRONLY, 0644);
        break;
    case STD_IN:
        out_fd = open(ast->data.ast_redir.right->data.ast_file.filename, O_RDONLY);
        in_fd = ionumber != -1 ? ionumber : STDIN_FILENO;
        break;
    case STD_OUT_END:
        in_fd = ionumber != -1 ? ionumber : STDOUT_FILENO;
        out_fd = open(ast->data.ast_redir.right->data.ast_file.filename, O_APPEND | O_WRONLY, 0644);
        break;
    case STD_IN_OUT:
        in_fd = ionumber != -1 ? ionumber : STDOUT_FILENO;
        out_fd = open(ast->data.ast_redir.right->data.ast_file.filename, O_CREAT | O_RDWR, 0666);
        break;
    case STD_ERR:
        in_fd = ionumber != -1 ? ionumber : STDOUT_FILENO;
        out_fd = open(ast->data.ast_redir.right->data.ast_file.filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        break;
    default:
        printf("Not dealing with it rn good luck\n");
        return 1;
    }
    if (out_fd == -1)
        return wrong_file(ast->data.ast_redir.right->data.ast_file.filename);

    int f = fork();
    if (f < 0)
        errx(1, "Bad fork");
    //child
    if (f == 0)
    {
        dup2(out_fd, in_fd);
        if (ast->data.ast_redir.type == STD_IN_OUT)
            dup2(out_fd, STDIN_FILENO);
        if (ast->data.ast_redir.type == STD_ERR)
            dup2(out_fd, STD_ERR);
        close(out_fd);
        return execute_tree(ast->data.ast_redir.left, describer);
    }
    else
    {
        int status;
        waitpid(f, &status, 0);
        close(out_fd);
        if (WIFEXITED(status))
        {
            int ex_st = WEXITSTATUS(status);
            return ex_st;
        }
    }
    return 0;
}
static int exec_loop(struct exec_arguments describer, struct ast *ast)
{
    struct ast_loop loop_struct = ast->data.ast_loop;
    int ans = 0;
    if (loop_struct.type == WHILE_LOOP)
    {
        while (execute_tree(loop_struct.cond, describer) == 0)
            ans = execute_tree(loop_struct.then_body, describer);
    }
    else if (loop_struct.type == UNTIL_LOOP)
    {
        while (execute_tree(loop_struct.cond, describer) != 0)
            ans = execute_tree(loop_struct.then_body, describer);
    }
    return ans;
}

static int exec_negation(struct exec_arguments describer, struct ast *ast)
{
    int ans = execute_tree(ast->data.ast_not.child, describer);
    if (ans == 0 || ans == 1)
        return ans == 0 ? 1 : 0;
    else
        return ans;
}

static int exec_list(struct exec_arguments describer, struct ast *ast)
{
    int ans = 0;
    for (size_t i = 0; i < ast->data.ast_list.nb_nodes; i++)
    {
        ans = execute_tree(ast->data.ast_list.list[i], describer);
    }
    return ans;
}

static int exec_operator(struct exec_arguments describer, struct ast *ast)
{
    int ans = execute_tree(ast->data.ast_operator.left, describer);
    if (ast->data.ast_operator.type == OP_OR)
        return ans == 0 ? ans : execute_tree(ast->data.ast_operator.right, describer);
    return ans != 0 ? ans : execute_tree(ast->data.ast_operator.right, describer);
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
    case AST_LOOP:;
        return exec_loop(describer, ast);
    case AST_REDIR:
        return exec_redir(describer, ast);
    case AST_NOT:
        return exec_negation(describer, ast);
    case AST_LIST:
        return exec_list(describer, ast);
    case AST_OPERATOR:
        return exec_operator(describer, ast);
    default:
        return -1;
        break;
    }
    return -1;
}
