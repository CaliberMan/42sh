#ifndef PARSER_H
#define PARSER_H

enum ast_type
{
    AST_IF,
    AST_ELSE,
    AST_COMMAND
};

/**
 * @param left The left node of the ast
 * @param right The right node of the ast
 * @param type The type of the node
 * @param command array of the command name and the args. command[0] = command name
 */
struct ast
{
    struct ast *left;
    struct ast *right;
    enum ast_type type;
    char **command;
};

/**
 * @brief Creates an empty ast node
 * @param ast_type The type of the ast node to be created
 * @return An empty ast node
 */
struct ast *init_ast(enum ast_type);

/**
 * @brief Frees an ast recursively
 * @param ast The ast to free
 */
void free_ast(struct ast *ast);

#endif /* ! PARSER_H */
