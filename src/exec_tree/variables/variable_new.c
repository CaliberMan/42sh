#include "variable.h"

void free_list_global(void)
{
    struct variable_list *list = get_global_list()->var_list;
    if (list != NULL)
    {
        struct variable_list *actual = list;
        while (actual)
        {
            struct variable_list *next = actual->next;
            free_single_var(actual);
            actual = next;
        }
    }
    struct function_list *li = get_global_list()->func_list;
    if (li != NULL)
    {
        struct function_list *actual = li;
        while (actual)
        {
            struct function_list *next = actual->next;
            free(actual->func->name);
            free(actual->func);
            free(actual);
            actual = next;
        }
    }
    free(get_global_list());
}
