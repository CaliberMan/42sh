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
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_EOF)
        return PARSER_ERROR;

    enum parser_status status = parse_and_or(ast, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    token = lexer_peek(lexer);
    while (token->type == TOKEN_COLON)
    {
        lexer_pop(lexer);

        struct ast *right;
        status = parse_and_or(&right, lexer);
        if (status == PARSER_ERROR)
            return PARSER_ERROR;

        // add the right node to the new one

        token = lexer_peek(lexer);
    }

    if (token->type == TOKEN_COLON)
    {
        // do something
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
