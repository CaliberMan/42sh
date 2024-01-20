#include "variable.h"

struct variable_list *begining_list = NULL;

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
