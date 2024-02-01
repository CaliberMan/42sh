#include "variable.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct variable_list *begining_list = NULL;

int expand_special(char *str, size_t j, struct exec_arguments *command,
                          size_t str_pos)
{
    char *special_args[] = {"@", "*", "?", "$", "#"};
    if (str[j+1] == '{')
        j++;
    for (size_t i = 0; i < 5; i++)
    {
        if (str[j + 1] == *special_args[i])
        {
            struct variable *ptr = find(special_args[i]);
            char *tmp = calloc(1, 1);
            char *new = NULL;
            if (ptr)
                new = replace_str(ptr->value, str, j, str + j+2);
            else
                new = replace_str(tmp, str, j, str + j + 2);
            free(tmp);
            free(command->args[str_pos]);
            command->args[str_pos] = new;
            return 0;
        }
    }
    return 1;
}

void free_list_variables(void)
{
    if (begining_list == NULL)
        return;
    struct variable_list *actual = begining_list;
    while (actual)
    {
        struct variable_list *next = actual->next;
        free_single_var(actual);
        actual = next;
    }
}

int update_variable(char *name, char *new_value)
{
    if (begining_list == NULL)
    {
        begining_list = make_variable(name, new_value);
        return 0;
    }
    struct variable_list *actual = begining_list;
    struct variable_list *prev = NULL;
    while (actual)
    {
        if (strcmp(actual->var->name, name) == 0)
        {
            free(actual->var->value);
            size_t len_var = strlen(new_value);
            actual->var->value = calloc(1, len_var + 1);
            memcpy(actual->var->value, new_value, len_var);
            return 0;
        }
        prev = actual;
        actual = actual->next;
    }

    prev->next = make_variable(name, new_value);
    return 0;
}

int unset_variable(char *name)
{
    struct variable_list *actual = begining_list;
    if (actual && strcmp(actual->var->name, name) == 0)
    {
        begining_list = begining_list->next;
        free_single_var(actual);
        return 0;
    }
    while (actual)
    {
        if (actual->next && strcmp(actual->next->var->name, name) == 0)
        {
            struct variable_list *to_rm = actual->next;
            actual->next = to_rm->next;
            free_single_var(to_rm);
            return 0;
        }
        actual = actual->next;
    }
    return 1;
}

struct variable *find(char *name)
{
    struct variable_list *actual = begining_list;
    while (actual)
    {
        if (strcmp(actual->var->name, name) == 0)
            return actual->var;
        actual = actual->next;
    }
    return NULL;
}

void init_variables(void)
{
    char base_str1[16] = {0};
    sprintf(base_str1, "%d", getpid());
    update_variable("$", base_str1);
    char base_str2[16] = {0};
    base_str2[0] = '0';
    update_variable("?", base_str2);
    char base_str3[1028] = {0};
    getcwd(base_str3, 1028);
    // they start with the same value
    update_variable("PWD", base_str3);
    update_variable("OLDPWD", base_str3);
    char base_str4[1028] = {0};
    srand(time(NULL));
    int r = rand() % 32768;
    sprintf(base_str4, "%d", r);
    update_variable("RANDOM", base_str4);
}

int variable_expansion(struct exec_arguments command)
{
    for (size_t i = 0; command.args[i]; i++)
    {
        size_t j = 0;
        char *str = command.args[i];
        while (str[j] && str[j] != '$')
            j++;
        if (!str[j])
            continue;

        // special variables
        int ans;
        if ((ans = expand_special(str, j, &command, i)) == 0)
            return ans;

        // environment variables
        size_t before_declaration = j;
        // we add one because we are on a dollar sign
        j++;
        if (str[j] == '{')
            j++;

        size_t variable_size = get_longest_valid_name(str, j);
        char *after_word = str + variable_size + j;
        // TEMPORARY CODE FOR OTHER VARIABLE
        char *get_var = calloc(1, variable_size + 1);
        memcpy(get_var, str + j, variable_size);
        if (strcmp(get_var, "RANDOM") == 0)
        {
            char new_rand[16] = {0};
            int r = rand() % 32768;
            sprintf(new_rand, "%d", r);
            update_variable("RANDOM", new_rand);
        }
        struct variable *p = find(get_var);
        free(get_var);
        char *tmp = calloc(1, 1);
        char *new = NULL;
        if (p)
            new = replace_str(p->value, str, before_declaration, after_word);
        else
            new = replace_str(tmp, str, before_declaration, after_word);
        free(tmp);
        free(command.args[i]);
        command.args[i] = new;
    }
    return 0;
}
