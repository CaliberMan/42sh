#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../exec_tree/exec_tree.h"
#include "../parser/input/input.h"
#include "../parser/utils.h"
#include "../utils/utils_main.h"
#include "stdlib.h"

static size_t get_arr_len(char **arr)
{
    size_t i = 0;
    while (arr[i])
        i++;
    return i;
}

int b_true(void)
{
    return 0;
}

int b_false(void)
{
    return 1;
}

static int litlle_helper(struct ast *ast, struct lexer *lexer,
                         struct exec_arguments command)
{
    fprintf(stderr, "%s: not parsable", command.args[1]);
    free_ast(ast);
    lexer_free(lexer);
    return 1;
}

static int little_helper2(struct exec_arguments command, char **var_list)
{
    int out = 0;
    for (size_t i = 2; i < 66; i++)
    {
        if (out == 0 && command.args[i])
        {
            char var_name[64];
            sprintf(var_name, "%zu", i - 1);
            struct variable *var = find_var(var_name);
            if (!var)
            {
                var_list[i - 2] = calloc(3, sizeof(char));
                strcpy(var_list[i - 2], "");
            }
            else
            {
                var_list[i - 2] = calloc(strlen(var->value) + 1, sizeof(char));
                strcpy(var_list[i - 2], var->value);
            }
            update_variable(var_name, command.args[i]);
        }
        else
        {
            var_list[i - 2] = NULL;
            out = 1;
        }
    }
    return 0;
}

int b_dot(struct exec_arguments command)
{
    struct lexer *lexer = file_to_lexer(command.args[1]);
    if (!lexer)
    {
        fprintf(stderr, "%s: not runnable", command.args[1]);
        return 1;
    }
    struct ast *ast;
    enum parser_status ps = parse_input(&ast, lexer);
    if (ps == PARSER_ERROR)
    {
        return litlle_helper(ast, lexer, command);
    }
    size_t cap = 64;
    char **var_list = calloc(cap, sizeof(char *));
    little_helper2(command, var_list);
    int ans = execute_tree(ast, command).value;
    for (size_t i = 0; i < cap; i++)
    {
        char var_name[64];
        sprintf(var_name, "%zu", i + 1);
        if (var_list[i] != NULL)
        {
            if (var_list[i][0] == 0)
                unset_variable(var_name);
            else
                update_variable(var_name, var_list[i]);
            free(var_list[i]);
        }
    }
    free(var_list);
    lexer_free(lexer);
    free_ast(ast);
    return ans;
}
int b_cd(struct exec_arguments command)
{
    char **args = command.args;
    if (get_arr_len(args) > 2)
    {
        fprintf(stderr, "%s\n", "Dont you dare do something like that");
        return 1;
    }
    char cur_dir[1028];
    getcwd(cur_dir, 1028);
    if (!args[1])
    {
        if (chdir("/") == -1)
        {
            fprintf(stderr, "%s\n", "Dont you dare do something like that");
            return 1;
        }
    }
    else if (strcmp(args[1], "-") == 0)
    {
        struct variable *var = find_var("OLDPWD");
        if (!var || chdir(var->value) == -1)
        {
            fprintf(stderr, "%s\n", "Dont you dare do something like that");
            return 1;
        }
    }
    else if (chdir(args[1]) == -1)
    {
        fprintf(stderr, "%s\n", "Dont you dare do something like that");
        return 1;
    }
    char new_dir[1028];
    getcwd(new_dir, 1028);
    update_variable("PWD", new_dir);
    update_variable("OLDPWD", cur_dir);
    return 0;
}

int b_echo(struct exec_arguments command)
{
    char **args = command.args;
    size_t i = 1;
    int newline = 1;
    int interpreted = 0;
    int out = 0;
    for (; args[i]; i++)
    {
        if (args[i][0] != '-')
            break;
        for (size_t j = 1; args[i][j]; j++)
        {
            if (args[i][j] == 'n')
                newline = 0;
            // use puts
            else if (args[i][j] == 'E')
                interpreted = 0;
            else if (args[i][j] == 'e')
                interpreted = 1;
            else
            {
                // just a string starting with '-'
                out = 1;
                break;
            }
        }
        if (out)
            break;
    }
    if (interpreted)
    {
        for (; args[i]; i++)
        {
            fputs(args[i], stdout);
            if (args[i + 1])
                printf(" ");
        }
    }
    else
    {
        for (; args[i]; i++)
        {
            printf("%s", args[i]);
            if (args[i + 1])
                printf(" ");
        }
    }
    if (newline)
        printf("\n");
    fflush(stdout);
    return 0;
}

int b_unset(struct exec_arguments command)
{
    size_t i = 1;
    if (strcmp("-v", command.args[i]) == 0)
    {
        i++;
        for (; command.args[i]; i++)
        {
            unset_variable(command.args[i]);
        }
    }
    else if (strcmp("-f", command.args[i]) == 0)
    {
        i++;
        for (; command.args[i]; i++)
            unset_function(command.args[i]);
    }
    else
    {
        for (; command.args[i]; i++)
        {
            int ans = unset_variable(command.args[i]);
            if (ans == 1)
            {
                unset_function(command.args[i]);
            }
        }
    }
    return 0;
}

int b_exit(struct exec_arguments command)
{
    size_t i = 1;
    int ans = 0;
    if (command.args[i])
        ans = atoi(command.args[i++]);
    if (command.args[i])
    {
        fprintf(stderr, "exit: too many arguments");
        return -1;
    }
    return ans < 0 ? 256 + (ans % 256) : ans % 256;
}

int b_break(struct exec_arguments command)
{
    size_t i = 1;
    int ans = 0;
    if (!command.args[i])
        return 0;
    ans = atoi(command.args[i++]);
    if (ans < 0)
    {
        fprintf(stderr, "continue: %i loop count out of range", ans);
        ans = -1;
    }
    if (command.args[i])
    {
        ans = -1;
        fprintf(stderr, "break: too many arguments");
    }
    return ans;
}

int b_continue(struct exec_arguments command)
{
    size_t i = 1;
    int ans = 0;
    if (!command.args[i])
        return 0;
    ans = atoi(command.args[i++]);
    if (ans < 0)
    {
        fprintf(stderr, "continue: %i loop count out of range", ans);
        ans = -1;
    }
    if (command.args[i])
    {
        ans = -1;
        fprintf(stderr, "continue: too many arguments");
    }
    return ans;
}
