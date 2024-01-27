#include "ast_list.h"
#include <stdlib.h>

#include "../ast.h"

void free_list(struct ast_list *list)
{
    for (size_t i = 0; i < list->capacity; i++)
        free_ast(list->list[i]);

    free(list);
}
