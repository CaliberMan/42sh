#include "input.h"

enum parser_status parse_input(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_NEWLINE || token->type == TOKEN_EOF)
    {
        token_free(token);
        return PARSER_OK;
    }

    token_free(token);
    enum parser_status status = parse_list(ast, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    token = lexer_peek(lexer);
    if (token->type != TOKEN_NEWLINE && token->type != TOKEN_EOF)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    token_free(token);
    return PARSER_OK;
}
