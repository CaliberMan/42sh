#include "parser_shell_cmd.h"

#include "if/parser_if.h"
#include "loops/parser_loops.h"

enum parser_status parse_shell_command(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_IF)
    {
        token_free(token);
        return parse_rule_if(ast, lexer);
    }

    if (token->type == TOKEN_WHILE)
    {
        token_free(token);
        return parse_while_until(ast, lexer, WHILE_LOOP);
    }

    if (token->type == TOKEN_UNTIL)
    {
        token_free(token);
        return parse_while_until(ast, lexer, UNTIL_LOOP);
    }

    token_free(token);
    return PARSER_UNKNOWN_TOKEN;
}
