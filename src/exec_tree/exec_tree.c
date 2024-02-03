#include "exec_tree.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "var_utils/var_utils.h"
#include "variables/variable.h"
#include "variables/variable_new.h"

int nb_loops = 0;

static int builtins_aux(struct exec_arguments command, struct ret_msg *ans)
{
    if (strcmp(command.args[0], "echo") == 0)
    {
        ans->value = b_echo(command);
        return 1;
    }
    else if (strcmp(command.args[0], "true") == 0)
    {
        ans->value = b_true();
        return 1;
    }
    else if (strcmp(command.args[0], "false") == 0)
    {
        ans->value = b_false();
        return 1;
    }
    else if (strcmp(command.args[0], "unset") == 0)
    {
        ans->value = b_unset(command);
        return 1;
    }
    else if (strcmp(command.args[0], "cd") == 0)
    {
        ans->value = b_cd(command);
        return 1;
    }
    else if (strcmp(command.args[0], "exit") == 0)
    {
        ans->value = b_exit(command);
        if (ans->value == -1)
            ans->value = 1;
        else
            ans->type = EXT;
        return 1;
    }
    else if (strcmp(command.args[0], ".") == 0)
    {
        ans->value = b_dot(command);
        if (ans->value == -1)
            ans->value = 1;
        return 1;
    }

    return 0;
}

static int builtins_aux_1(struct exec_arguments command, struct ret_msg *ans)
{
    if (strcmp(command.args[0], "break") == 0)
    {
        if (nb_loops == 0)
        {
            fprintf(
                stderr,
                "break: only meaningfull if a 'while', 'for' or 'until' loop");
            return 1;
        }
        ans->value = b_break(command);
        ans->type = BRK;
        if (ans->value == -1)
        {
            ans->value = 1;
            ans->type = EXT;
        }
        else
        {
            ans->type = BRK;
        }
        if (ans->value > nb_loops)
            ans->value = nb_loops;
        return 1;
    }
    else if (strcmp(command.args[0], "continue") == 0)
    {
        if (nb_loops == 0)
        {
            fprintf(stderr,
                    "continue: only meaningfull if a 'while', 'for' or 'until' "
                    "loop");
            return 1;
        }
        ans->value = b_continue(command);
        if (ans->value == -1)
        {
            ans->value = 1;
            ans->type = EXT;
        }
        else
        {
            ans->type = CTN;
        }
        if (ans->value > nb_loops)
            ans->value = nb_loops;
        return 1;
    }
    return 0;
}

static struct ret_msg check_builtins(struct exec_arguments command)
{
    struct ret_msg ans;
    ans.type = VAL;
    ans.value = 0;
    if (builtins_aux(command, &ans))
        return ans;
    if (builtins_aux_1(command, &ans))
        return ans;
    // function check
    struct function *func = NULL;
    if ((func = find_func(command.args[0])))
    {
        struct global_list *prev_context = get_global_list();
        struct exec_arguments passing_args;
        char *buf[] = { "42sh",          command.args[0], command.args[1],
                        command.args[2], command.args[3], command.args[4],
                        command.args[5], command.args[6], command.args[7],
                        command.args[8], command.args[9] };

        passing_args.args = buf;
        init_variables(passing_args.args);
        ans = execute_tree(func->body, command);

        free_list_global();
        set_global_list(prev_context);
        return ans;
    }
    ans.value = exec(command);
    if (ans.value != 0 && ans.type != 1)
        ans.type = ERR;
    return ans;
}

static struct ret_msg exec_cmd(struct exec_arguments describer, struct ast *ast)
{
    struct ret_msg ans;
    ans.type = VAL;
    char **args = ast->data.ast_cmd.words;
    // copy commands for expansion
    size_t len_to_pass = 0;
    for (size_t i = 0; args[i]; i++)
        len_to_pass++;
    char **to_pass = calloc(len_to_pass + 1, sizeof(char *));
    for (size_t i = 0; args[i]; i++)
    {
        char *ar = calloc(1, strlen(args[i]) + 1);
        memcpy(ar, args[i], strlen(args[i]));
        to_pass[i] = ar;
    }
    describer.args = to_pass;
    ans.value = variable_expansion(describer);
    if (ans.value != 0)
    {
        ans.type = ERR;
        ans.value = 1;
        return ans;
    }

    ans = check_builtins(describer);
    for (size_t i = 0; to_pass[i]; i++)
        free(to_pass[i]);
    free(to_pass);

    char buf[16];
    sprintf(buf, "%d", ans.value);
    update_variable("?", buf);
    return ans;
}

static struct ret_msg exec_if(struct exec_arguments describer, struct ast *ast)
{
    struct ret_msg ans;
    ans.type = VAL;
    struct ast_if if_struct = ast->data.ast_if;
    ans = execute_tree(if_struct.cond, describer);
    if (ans.type == EXT || ans.type == BRK || ans.type == CTN)
        return ans;
    if (ans.value == 0)
    {
        ans = execute_tree(if_struct.then_body, describer);
        if (ans.type == EXT || ans.type == BRK || ans.type == CTN)
            return ans;
    }
    else if (if_struct.else_body != NULL)
    {
        ans = execute_tree(if_struct.else_body, describer);
        if (ans.type == EXT || ans.type == BRK || ans.type == CTN)
            return ans;
    }
    else
        ans.value = 0;
    return ans;
}

static struct ret_msg pipe_aux(struct exec_arguments describer, struct ast *ast)
{
    struct ret_msg ans;
    ans.type = VAL;
    ans.value = 0;
    close(describer.pipe_fds[0]);
    dup2(describer.pipe_fds[1], STDOUT_FILENO);
    ans = execute_tree(ast->data.ast_pipe.left_arg, describer);
    struct exec_arguments exit_args;
    char buf[16] = { 0 };
    sprintf(buf, "%d", ans.value);
    char *ar[] = { "exit", buf, 0 };
    exit_args.args = ar;
    ans.value = b_exit(exit_args);
    if (ans.value == -1)
        ans.value = 1;
    else
        ans.type = EXT;
    return ans;
}

static struct ret_msg exec_pipe(struct exec_arguments describer,
                                struct ast *ast)
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
        return pipe_aux(describer, ast);
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

static int find_out(int *in_fd, int *out_fd, int ionumber, struct ast *ast)
{
    switch (ast->data.ast_redir.type)
    {
    case STD_OUT:
    case STD_RIGHT_ARROW_PIPE:
        *in_fd = ionumber != -1 ? ionumber : STDOUT_FILENO;
        *out_fd = open(ast->data.ast_redir.right->data.ast_file.filename,
                       O_CREAT | O_TRUNC | O_WRONLY, 0644);
        break;
    case STD_IN:
        *out_fd =
            open(ast->data.ast_redir.right->data.ast_file.filename, O_RDONLY);
        *in_fd = ionumber != -1 ? ionumber : STDIN_FILENO;
        break;
    case STD_OUT_END:
        *in_fd = ionumber != -1 ? ionumber : STDOUT_FILENO;
        *out_fd = open(ast->data.ast_redir.right->data.ast_file.filename,
                       O_APPEND | O_WRONLY, 0644);
        break;
    case STD_IN_OUT:
        *in_fd = ionumber != -1 ? ionumber : STDOUT_FILENO;
        *out_fd = open(ast->data.ast_redir.right->data.ast_file.filename,
                       O_CREAT | O_RDWR, 0666);
        break;
    case STD_ERR:
        *in_fd = ionumber != -1 ? ionumber : STDOUT_FILENO;
        *out_fd = open(ast->data.ast_redir.right->data.ast_file.filename,
                       O_WRONLY | O_CREAT | O_TRUNC, 0666);
        break;
    default:
        return 1;
    }
    return 0;
}

static struct ret_msg exec_redir(struct exec_arguments describer,
                                 struct ast *ast)
{
    struct ret_msg ans;
    ans.type = VAL;
    ans.value = 0;
    int in_fd;
    int out_fd;
    int ionumber = ast->data.ast_redir.ioNumber;
    if (find_out(&in_fd, &out_fd, ionumber, ast) == 1)
    {
        ans.value = 1;
        return ans;
    }
    if (out_fd == -1)
        return wrong_file(ast->data.ast_redir.right->data.ast_file.filename);

    int saved = dup(in_fd);
    dup2(out_fd, in_fd);
    int saved_2 = -1;
    if (ast->data.ast_redir.type == STD_IN_OUT)
    {
        saved_2 = dup(STDIN_FILENO);
        dup2(out_fd, STDIN_FILENO);
    }
    if (ast->data.ast_redir.type == STD_ERR)
    {
        saved_2 = dup(STD_ERR);
        dup2(out_fd, STD_ERR);
    }
    close(out_fd);
    ans = execute_tree(ast->data.ast_redir.left, describer);
    dup2(saved, in_fd);
    if (ast->data.ast_redir.type == STD_IN_OUT)
    {
        dup2(saved_2, STDIN_FILENO);
        close(saved_2);
    }
    if (ast->data.ast_redir.type == STD_ERR)
    {
        dup2(saved_2, STD_ERR);
        close(saved_2);
    }
    close(saved);
    return ans;
}

// returns 0 if it should stop the loop, 1 if it should continue and 2 if it
// should run the condition will uptdate the break and continue values
static int stop_loop_check(struct ret_msg *msg)
{
    int ans = 1;
    switch (msg->type)
    {
    case BRK:
        ans = 0;
        if (msg->value > 0)
            msg->value--;
        else if (msg->value == 0)
            msg->type = VAL;
        break;
    case EXT:
        ans = 0;
        break;
    case CTN:
        ans = 0;
        if (msg->value == 0)
        {
            ans = 2;
            msg->type = VAL;
        }
        else
            msg->value--;
        break;
    default:
        return 1;
    }
    return ans;
}

static struct ret_msg exec_while(struct exec_arguments describer,
                                 struct ast *ast, struct ret_msg ans)
{
    struct ast_loop loop_struct = ast->data.ast_loop;
    struct ret_msg ret = execute_tree(loop_struct.cond, describer);
    int stop = stop_loop_check(&ret);
    if (stop == 0)
    {
        nb_loops--;
        return ret;
    }
    if (stop == 2)
        ret = execute_tree(loop_struct.cond, describer);
    while (ret.type == VAL && ret.value == 0)
    {
        ans = execute_tree(loop_struct.then_body, describer);
        stop = stop_loop_check(&ans);
        if (stop == 0)
        {
            nb_loops--;
            return ret;
        }
        ret = execute_tree(loop_struct.cond, describer);
    }
    nb_loops--;
    return ans;
}

static struct ret_msg exec_until(struct exec_arguments describer,
                                 struct ast *ast, struct ret_msg ans)
{
    struct ast_loop loop_struct = ast->data.ast_loop;
    struct ret_msg ret = execute_tree(loop_struct.cond, describer);
    int stop = stop_loop_check(&ret);
    if (stop == 0)
    {
        nb_loops--;
        return ret;
    }
    if (stop == 2)
        ret = execute_tree(loop_struct.cond, describer);
    while (ret.value == 1)
    {
        ans = execute_tree(loop_struct.then_body, describer);
        stop = stop_loop_check(&ans);
        if (stop == 0)
        {
            nb_loops--;
            return ans;
        }
        ret = execute_tree(loop_struct.cond, describer);
    }
    nb_loops--;
    return ans;
}

static struct ret_msg exec_for(struct exec_arguments describer, struct ast *ast,
                               struct ret_msg ans)
{
    struct ast_loop loop_struct = ast->data.ast_loop;
    update_variable(loop_struct.var_name, "");
    // actully something inside
    if (loop_struct.cond)
    {
        char **l = loop_struct.cond->data.ast_cmd.words;
        // copy commands for expansion
        size_t len_to_pass = 0;
        for (size_t i = 0; l[i]; i++)
            len_to_pass++;
        char **to_pass = calloc(len_to_pass + 1, sizeof(char *));
        for (size_t i = 0; l[i]; i++)
        {
            char *ar = calloc(1, strlen(l[i]) + 1);
            memcpy(ar, l[i], strlen(l[i]));
            to_pass[i] = ar;
        }
        struct exec_arguments tmp_desc = { 0 };
        tmp_desc.args = to_pass;
        variable_expansion(tmp_desc);

        for (size_t i = 0; to_pass[i]; i++)
        {
            char *word = to_pass[i];
            update_variable(loop_struct.var_name, word);
            ans = execute_tree(loop_struct.then_body, describer);
            int stop = stop_loop_check(&ans);
            if (stop == 0)
            {
                nb_loops--;
                for (size_t i = 0; to_pass[i]; i++)
                    free(to_pass[i]);
                free(to_pass);
                return ans;
            }
            if (stop == 2)
                continue;
        }
        for (size_t i = 0; to_pass[i]; i++)
            free(to_pass[i]);
        free(to_pass);
    }
    nb_loops--;
    return ans;
}

static struct ret_msg exec_loop(struct exec_arguments describer,
                                struct ast *ast)
{
    struct ast_loop loop_struct = ast->data.ast_loop;
    struct ret_msg ans;
    ans.type = VAL;
    ans.value = 0;
    nb_loops++;
    if (loop_struct.type == WHILE_LOOP)
    {
        return exec_while(describer, ast, ans);
    }
    else if (loop_struct.type == UNTIL_LOOP)
    {
        return exec_until(describer, ast, ans);
    }
    else
    {
        return exec_for(describer, ast, ans);
    }
    nb_loops--;
    return ans;
}

static struct ret_msg exec_negation(struct exec_arguments describer,
                                    struct ast *ast)
{
    struct ret_msg ans;
    ans = execute_tree(ast->data.ast_not.child, describer);
    if (ans.type == VAL || ans.type == ERR)
    {
        ans.value = ans.value == 0 ? 1 : 0;
        return ans;
    }
    else
        return ans;
}

static struct ret_msg exec_list(struct exec_arguments describer,
                                struct ast *ast)
{
    struct ret_msg ans;
    ans.value = 0;
    ans.type = VAL;
    for (size_t i = 0; i < ast->data.ast_list.nb_nodes; i++)
    {
        ans = execute_tree(ast->data.ast_list.list[i], describer);
        if (ans.type == EXT || ans.type == BRK || ans.type == CTN)
            return ans;
    }
    return ans;
}

static struct ret_msg exec_operator(struct exec_arguments describer,
                                    struct ast *ast)
{
    struct ret_msg ans = execute_tree(ast->data.ast_operator.left, describer);
    if (ans.type == EXT || ans.type == BRK || ans.type == CTN)
        return ans;
    if (ast->data.ast_operator.type == OP_OR)
        return ans.value == 0
            ? ans
            : execute_tree(ast->data.ast_operator.right, describer);
    return ans.value != 0
        ? ans
        : execute_tree(ast->data.ast_operator.right, describer);
}
static struct ret_msg exec_variable(struct ast *ast)
{
    struct ast_variable variable_struct = ast->data.ast_variable;
    update_variable(variable_struct.name,
                    variable_struct.value->data.ast_cmd.words[0]);
    struct ret_msg ans;
    ans.value = 0;
    ans.type = VAL;
    return ans;
}

static struct ret_msg exec_subshell(struct exec_arguments describer,
                                    struct ast *ast)
{
    struct ret_msg ans;
    ans.value = 0;
    ans.type = VAL;

    struct ast_sub subshell_struct = ast->data.ast_sub;
    // set up forking
    int fork_fd = fork();
    if (fork_fd < 0)
        errx(1, "%s\n", "Bad fork");

    describer.child_process = fork_fd;
    if (fork_fd == 0)
    {
        ans = execute_tree(subshell_struct.list, describer);
        struct exec_arguments exit_args;
        char buf[16] = { 0 };
        sprintf(buf, "%d", ans.value);
        char *ar[] = { "exit", buf, 0 };
        exit_args.args = ar;
        ans.value = b_exit(exit_args);
        if (ans.value == -1)
            ans.value = 1;
        else
            ans.type = EXT;
        return ans;
    }
    else
    {
        int status;
        waitpid(fork_fd, &status, 0);
        if (WIFEXITED(status))
        {
            int ex_st = WEXITSTATUS(status);
            ans.value = ex_st;
            if (ex_st == 127)
                ans.type = ERR;
            return ans;
        }
    }
    return ans;
}
static struct ret_msg exec_case(struct exec_arguments describer,
                                struct ast *ast)
{
    struct ast_case case_struct = ast->data.ast_case;
    char *matcher = case_struct.expr;
    struct ast_list list = case_struct.cases_list->data.ast_list;
    for (size_t i = 0; i < list.nb_nodes; i++)
    {
        char **current_pattern =
            list.list[i]->data.ast_pattern.pattern->data.ast_cmd.words;
        for (size_t k = 0; current_pattern[k]; k++)
        {
            char *cur_word = current_pattern[k];
            if (strcmp("*", cur_word) == 0)
                return execute_tree(list.list[i]->data.ast_pattern.statement,
                                    describer);
            // found a match
            if (strcmp(matcher, cur_word) == 0)
                return execute_tree(list.list[i]->data.ast_pattern.statement,
                                    describer);
        }
    }

    struct ret_msg ans;
    ans.value = 0;
    ans.type = VAL;
    return ans;
}

static struct ret_msg exec_function(struct ast *ast)
{
    struct ast_func func_struct = ast->data.ast_func;
    update_function(func_struct.name, func_struct.body);
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
    case AST_FUNCTION:
        return exec_function(ast);
    case AST_SUBSHELL:
        return exec_subshell(describer, ast);
    case AST_CASE:
        return exec_case(describer, ast);
    default:
        ans.type = ERR;
        ans.value = -1;
        return ans;
        break;
    }
}
