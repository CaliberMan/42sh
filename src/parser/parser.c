#include "parser.h"

enum parser_status parse_input(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_NEWLINE || token->type == TOKEN_EOF)
        return PARSER_OK;

    enum parser_status status = parse_list(ast, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    token = lexer_peek(lexer);
    if (token->type != TOKEN_NEWLINE && token->type != TOKEN_EOF)
        return PARSER_ERROR;

    return PARSER_OK;
}

enum parser_status parse_list(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = parse_and_or(ast, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    token = lexer_peek(lexer);
    while (token->type == TOKEN_COLON)
    {
        lexer_pop(lexer);

        struct ast *left;
        status = parse_and_or(&left, lexer);

        // means the next token is not in and_or rule
        if (status == PARSER_ERROR)
            return PARSER_OK;

        // add the right node to the new one
        *ast->left = left;
        token = lexer_peek(lexer);
    }

    return PARSER_OK;
}

enum parser_status parse_and_or(struct ast **ast, struct lexer *lexer)
{
    return parse_pipeline(ast, lexer);
}

enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer)
{
    return parse_command(ast, lexer);
}

enum parser_status parse_command(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_WORD)
        return parse_simple_command(ast, lexer);

    return parse_shell_command(ast, lexer);
}

enum parser_status parse_simple_command(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
        return PARSER_ERROR;

    struct ast *command_node = init_ast(AST_COMMAND);
    command_node->command[0] = token->data;

    command->left = *ast;

    lexer_pop(lexer);
    int index = 1;

    while (1)
    {
        struct ast *next_word;
        enum parser_status status = parse_element(&next_word, lexer);
        if (status == PARSER_ERROR)
            return PARSER_OK;

        command_node->command[index++] = next_word->command[0];
    }

    *ast = command_node;
    return PARSER_OK;
}

enum parser_status parse_element(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
        return PARSER_ERROR;

    struct ast *node = init_ast(AST_COMMAND);
    node->command[0] = token->data;
    *ast = node;

    return PARSER_OK;
}

enum parser_status parse_shell_command(struct ast **ast, struct lexer *lexer)
{
    return parser_rule_if(ast, lexer);
}

enum parser_status parse_rule_if(struct ast **ast, struct lexer *lexer)
{
}

enum parser_status parse_else_clause(struct ast **ast, struct lexer *lexer);
{
}

enum parser_status parse_command_list(struct ast **ast, struct lexer *lexer)
{
}
