#ifndef AST_LIST_H
#define AST_LIST_H

#include <stddef.h>

struct ast_list
{
    struct ast **list;
    size_t capacity;
};

/**
 * @brief Frees a list of asts
 * @param list A list of asts
 */
void free_list(struct ast_list *list);

#endif /* ! AST_LIST_H */
