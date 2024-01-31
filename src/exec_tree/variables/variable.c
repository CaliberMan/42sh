#include "variable.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct variable_list *begining_list = NULL;

void free_list_variables(void)
{
    if (begining_list == NULL)
        return;
    struct variable_list *actual = begining_list;
    while (actual)
    {
        struct variable_list *next = actual->next;
        if (actual->var->type == STR)
            free(actual->var->data.string);
        free(actual->var->name);
        free(actual->var);
        free(actual);
        actual = next;
    }
}

int update_variable(char *name, enum var_type type, union var_data data)
{
    size_t len = strlen(name) + 1;
    if (begining_list == NULL)
    {
        begining_list = calloc(1, sizeof(struct variable_list));
        if (!begining_list)
            return 1;
        begining_list->next = NULL;

        struct variable *new = calloc(1, sizeof(struct variable));
        if (!new)
            return 1;
        new->data = data;
        if (type == STR)
        {
            size_t len_var = strlen(data.string);
            new->data.string = calloc(1, len_var + 1);
            memcpy(new->data.string, data.string, len_var);
            free(data.string);
        }
        new->type = type;
        new->name = calloc(1, len);
        strncpy(new->name, name, len);
        begining_list->var = new;
        return 0;
    }
    struct variable_list *actual = begining_list;
    while (actual->next)
    {
        if (strcmp(actual->var->name, name) == 0)
        {
            actual->var->data = data;
            actual->var->type = type;
            return 0;
        }
        actual = actual->next;
    }
    struct variable_list *new_l = calloc(1, sizeof(struct variable_list));
    if (!new_l)
        return 1;
    new_l->next = NULL;
    struct variable *new_var = calloc(1, sizeof(struct variable));
    if (!new_var)
        return 1;
    new_var->data = data;
    new_var->type = type;
    new_var->name = calloc(1, len + 1);
    memcpy(new_var->name, name, len);
    actual->next = new_l;
    new_l->var = new_var;
    return 0;
}

int unset_variable(char *name)
{
    struct variable_list *actual = begining_list;
    if (actual && strcmp(actual->var->name, name) == 0)
    {
        begining_list = begining_list->next;
        if (actual->var->type == STR)
            free(actual->var->data.string);
        free(actual->var);
        free(actual);
        return 0;
    }
    while (actual)
    {
        if (actual->next && strcmp(actual->next->var->name, name) == 0)
        {
            struct variable_list *to_rm = actual->next;
            actual->next = to_rm->next;
            if (to_rm->var->type == STR)
                free(to_rm->var->data.string);
            free(to_rm->var);
            free(to_rm);
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

static size_t get_longest_valid_name(char *str, size_t i)
{
    size_t j = 0;
    while (str[j + i] && isalnum(str[j + i]))
        j++;
    return j;
}

void init_variables(void)
{
    char *base_str1 = calloc(1, 16);
    sprintf(base_str1, "%d", getpid());
    union var_data x;
    x.string = base_str1;
    update_variable("$", STR, x);
    char *base_str2 = calloc(1, 16);
    base_str2[0] = '0';
    union var_data y;
    y.string = base_str2;
    update_variable("?", STR, y);
}

static char *replace_str(char *ptr, char *str, size_t before_declaration,
                         char *after_word)
{
    if (*after_word == '}')
        after_word++;
    size_t ptr_len = strlen(ptr);
    size_t str_len = strlen(str);
    size_t after_len = strlen(after_word);
    char *new_guy = calloc(1, ptr_len + str_len);
    memcpy(new_guy, str, before_declaration);
    memcpy(new_guy + before_declaration, ptr, ptr_len);
    memcpy(new_guy + before_declaration + ptr_len, after_word, after_len);
    return new_guy;
}

static int expand_special(char *str, size_t j, struct exec_arguments *command,
                          size_t str_pos)
{
    char *special_args[] = { "@", "*", "?", "$", "#" };
    if (str[j + 1] == '{')
        j++;
    for (size_t i = 0; i < 5; i++)
    {
        if (str[j + 1] == *special_args[i])
        {
            struct variable *ptr = find(special_args[i]);
            char *tmp = calloc(1, 1);
            char *new = NULL;
            if (ptr)
                new = replace_str(ptr->data.string, str, j, str + j + 2);
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

        const char *env_var[] = { "OLDPWD", "PWD", "IFS" };
        size_t variable_size = get_longest_valid_name(str, j);
        char *ptr = NULL;
        char *after_word = str + variable_size + j;
        int out = 0;
        for (size_t k = 0; k < 3; k++)
        {
            if (strncmp(str + j, env_var[k], variable_size) == 0)
            {
                char *tmp = calloc(1, 1);
                char *new = NULL;
                if ((ptr = getenv(env_var[k])))
                    new = replace_str(ptr, str, before_declaration, after_word);
                else
                    new = replace_str(tmp, str, before_declaration, after_word);
                free(tmp);
                free(command.args[i]);
                command.args[i] = new;
                out = 1;
                break;
            }
        }
        if (out == 1)
            continue;
        // TEMPORARY CODE FOR OTHER VARIABLE
        char *get_var = calloc(1, variable_size + 1);
        memcpy(get_var, str + j, variable_size);
        struct variable *p = find(get_var);
        free(get_var);
        char *tmp = calloc(1, 1);
        char *new = NULL;
        if (ptr)
            new = replace_str(p->data.string, str, j, str + j + 2);
        else
            new = replace_str(tmp, str, j, str + j + 2);
        free(tmp);
        free(command.args[i]);
        command.args[i] = new;
    }
    return 0;
}
