#ifndef AST_CMD_H
#define AST_CMD_H

#include <stddef.h>

struct ast_cmd
{
    char **words; // array of words
    size_t capacity; // size of words
};

/**
 * @brief Creates an empty ast cmd node
 * @return An empty ast cmd node
 */
void init_words(struct ast_cmd *ast);

/**
 * @brief Doubles the size of the array words in ast_cmd
 */
void realloc_words(struct ast_cmd *ast);

/**
 * @brief Frees the array words in ast_cmd
 */
void free_words(struct ast_cmd *ast);

#endif /* ! AST_CMD_H */
