#include "exec_tree.h"
#include "variables/variable.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int check_builtins(struct exec_arguments command)
{
    if (strcmp(command.args[0], "echo") == 0)
        return b_echo(command);
    else if (strcmp(command.args[0], "true") == 0)
        return b_true();
    else if (strcmp(command.args[0], "false") == 0)
        return b_false();
    else if (strcmp(command.args[0], "unset") == 0)
        return b_unset(command);
    else
        return exec(command);
}

static struct ret_msg exec_cmd(struct exec_arguments describer, struct ast *ast)
{
    struct ret_msg ans;
    ans.type = VAL;
    struct ast_cmd cmd_struct = ast->data.ast_cmd;
    describer.args = cmd_struct.words;
    ans.value = variable_expansion(describer);
    if (ans.value != 0)
    {
        ans.type = ERR;
        ans.value = 1;
        return ans;
    }
    ans.value = check_builtins(describer);
    if (ans.value != 0 && ans.value != 1)
        ans.type = ERR;
    return ans;
}

static struct ret_msg exec_if(struct exec_arguments describer, struct ast *ast)
{
    struct ret_msg ans;
    ans.type = VAL;
    struct ast_if if_struct = ast->data.ast_if;
    ans = execute_tree(if_struct.cond, describer);
    if (ans.value == 0)
        ans = execute_tree(if_struct.then_body, describer);
    else if (if_struct.else_body != NULL)
        ans = execute_tree(if_struct.else_body, describer);
    else
        ans.value = 0;
    return ans;
}

static struct ret_msg exec_pipe(struct exec_arguments describer, struct ast *ast)
{
    struct ret_msg ans;
    ans.type = VAL;
    ans.value = 0;
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
            {
                ans.type = ERR;
                ans.value = 127;
            }
            else if (ex_st == 1)
            {
                ans.value = 1;
                return ans;
            }
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

static struct ret_msg wrong_file(char *name)
{
    fprintf(stderr, "%s: No such file or directory", name);
    struct ret_msg ans;
    ans.type = VAL;
    ans.value = 1;
    return ans;
}

static struct ret_msg exec_redir(struct exec_arguments describer, struct ast *ast)
{
    struct ret_msg ans;
    ans.type = VAL;
    ans.value = 0;
    int in_fd;
    int out_fd;
    int ionumber = ast->data.ast_redir.ioNumber;
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
        ans.value = 1;
        ans.type = ERR;
        return ans;
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
            ans.value = WEXITSTATUS(status);
            if (ans.value != 0 && ans.value != 1)
            {
                ans.type = ERR;
            }
            return ans;
        }
    }
    return ans;
}
static struct ret_msg exec_loop(struct exec_arguments describer, struct ast *ast)
{
    struct ast_loop loop_struct = ast->data.ast_loop;
    struct ret_msg ans;
    ans.type = VAL;
    ans.value = 0;
    if (loop_struct.type == WHILE_LOOP)
    {
        struct ret_msg ret = execute_tree(loop_struct.cond, describer);
        while (ret.type == VAL && ret.value == 0)
        {
            ans = execute_tree(loop_struct.then_body, describer);
            ret = execute_tree(loop_struct.cond, describer);
        }
    }
    else if (loop_struct.type == UNTIL_LOOP)
    {
        struct ret_msg ret = execute_tree(loop_struct.cond, describer);
        while (ret.type == VAL && ret.value == 0)
        {
            ans = execute_tree(loop_struct.then_body, describer);
            ret = execute_tree(loop_struct.cond, describer);
        }
    }
    else
    {
        // for looop
        update_variable(loop_struct.var_name, "");
        for (size_t i = 0; loop_struct.cond->data.ast_cmd.words[i]; i++)
        {
            update_variable(loop_struct.var_name, loop_struct.cond->data.ast_cmd.words[i]);
            execute_tree(loop_struct.then_body, describer);
        }
    }
    return ans;
}

static struct ret_msg exec_negation(struct exec_arguments describer, struct ast *ast)
{
    struct ret_msg ans;
    ans = execute_tree(ast->data.ast_not.child, describer);
    if (ans.type == VAL)
    {
        ans.value = ans.value == 0 ? 1 : 0;
        return ans;
    }
    else
        return ans;
}

static struct ret_msg exec_list(struct exec_arguments describer, struct ast *ast)
{
    struct ret_msg ans;
    ans.value = 0;
    ans.type = VAL;
    for (size_t i = 0; i < ast->data.ast_list.nb_nodes; i++)
    {
        ans = execute_tree(ast->data.ast_list.list[i], describer);
    }
    return ans;
}

static struct ret_msg exec_operator(struct exec_arguments describer, struct ast *ast)
{
    struct ret_msg ans = execute_tree(ast->data.ast_operator.left, describer);
    if (ast->data.ast_operator.type == OP_OR)
        return ans.value == 0 ? ans : execute_tree(ast->data.ast_operator.right, describer);
    return ans.value != 0 ? ans : execute_tree(ast->data.ast_operator.right, describer);
}
static struct ret_msg exec_variable(struct ast *ast)
{
    struct ast_variable variable_struct = ast->data.ast_variable;
    update_variable(variable_struct.name, variable_struct.value->data.ast_cmd.words[0]);
    struct ret_msg ans;
    ans.value = 0;
    ans.type = VAL;
    return ans;
}

struct ret_msg execute_tree(struct ast *ast, struct exec_arguments describer)
{
    struct ret_msg ans;
    ans.type = VAL;
    ans.value = 0;
    if (!ast)
        return ans;
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
    case AST_VARIABLE:
        return exec_variable(ast);
    default:
        ans.type = ERR;
        ans.value = -1;
        return ans;
        break;
    }
}
