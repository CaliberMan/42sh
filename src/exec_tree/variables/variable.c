#include "variable.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct variable_list *begining_list = NULL;

static size_t int_size(int x)
{
    size_t i = 1;
    while (x > 0)
    {
        x /= 10;
        i++;
    }
    return i;
}

//returns 0 if everything went well
//returns 1 if the variable doesn't exist
//returns -1 if something went really wrong (allocation/type error)
int check_variable(struct exec_arguments command)
{
    size_t i = 0;
    while(command.args[i] != NULL)
    {
        if (command.args[i][0] == '$')
        {
            struct variable *var = find(command.args[i] + 1);
            if (var == NULL)
                return 1;
            free(command.args[i]);
            switch(var->type)
            {
            case STR:
                command.args[i] = calloc(1, strlen(var->data.string));
                if (command.args[i] == NULL)
                    return -1;
                strcpy(command.args[i], var->data.string);
                break;
            case INT:
                command.args[i] = calloc(1, int_size(var->data.integer) + 1);
                if (command.args[i] == NULL)
                    return -1;
                sprintf(command.args[i], "%d", var->data.integer);
                break;
            case CHAR:
                command.args[i] = calloc(2, 1);
                if (command.args[i] == NULL)
                    return -1;
                command.args[i][0] = var->data.character;
                break;
            case FLOAT:
                command.args[i] = calloc(1, 64); //Default value don't know if
                                                 //we have to keep the float
                if (command.args[i] == NULL)
                    return -1;
                sprintf(command.args[i], "%f", var->data.floatable);
                break;
            default:
                return -1;
            }
        }
        i++;
    }
    return 0;
}

void free_list(void)
{
    if (begining_list == NULL)
        return;
    struct variable_list *actual = begining_list;
    while (actual)
    {
        struct variable_list *next = actual->next;
        if (actual->var->type == STR)
            free(actual->var->data.string);
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
        new->type = type;
        new->name = calloc(1, len);
        strncpy(new->name, name, len);

        begining_list->var = new;
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
    new_var->name = calloc(1, len);
    strncpy(new_var->name, name, len);
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

    char *t = calloc(1, 10);
    sprintf(t, "%d", getpid());
    union var_data x;
    x.string = t;
    update_variable("$", STR, x);
}

static char *replace_str(char *ptr, char *str, size_t before_declaration, char * after_word)
{
    if (*after_word == '}')
        after_word++;
    char *new_guy = calloc(1, strlen(ptr) + strlen(str));
    memcpy(new_guy, str, before_declaration);
    memcpy(new_guy + before_declaration, ptr, strlen(ptr));
    memcpy(new_guy + before_declaration + strlen(ptr), after_word, strlen(after_word));
    return new_guy;
}

static int expand_special(char *str, size_t j)
{
    switch (str[j+1])
    {
        case '@':
            break;
        case '*':
            break;
        case '?':
            break;
        case '$':
            break;
        case '#':
            break;
        default:
            break;
    }
    return 0;
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
        if (expand_special(str, j))
            continue;

        // environment variables
        size_t before_declaration = j;
        // we add one because we are on a dollar sign
        j++;
        if (str[j] == '{')
            j++;
        const char *env_var[] = {"OLDPWD", "PWD", "IFS"};
        size_t variable_size = get_longest_valid_name(str, j);
        char *ptr = NULL;
        char *after_word = str + variable_size + j;
        int out = 0;
        for (size_t k = 0; k < 3; k++)
        {
            if (strncmp(str + j, env_var[k], variable_size) == 0)
            {
                char *tmp = calloc(1,1);
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
        char *tmp = calloc(1,1);
        char *new = replace_str(tmp, str, before_declaration, after_word);
        free(tmp);
        free(command.args[i]);
        command.args[i] = new;
    }
    return 0;
}
