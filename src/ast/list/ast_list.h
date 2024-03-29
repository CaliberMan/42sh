#ifndef AST_LIST_H
#define AST_LIST_H

#include <stddef.h>

struct ast_list
{
    struct ast **list;
    size_t capacity;
    size_t nb_nodes;
};

/**
 * @brief Frees a list of asts
 * @param list A list of asts
 */
void free_ast_list(struct ast_list *list);

/**
 * @brief Callocs an empty list ast
 * @param ast The ast in which to create the list
 */
void init_list(struct ast_list *ast);

/**
 * @brief doubles the list
 * @param ast The ast in which the list is
 */
void double_list_size(struct ast_list *ast);

/**
 * @brief adds an ast to the list
 * @param ast The ast list
 * @param elm The elm to add
 * @param index The index to add to
 */
void add_ast(struct ast_list *ast, struct ast *elm, size_t *index);

#endif /* ! AST_LIST_H */
