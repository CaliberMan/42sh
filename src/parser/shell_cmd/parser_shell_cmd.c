#include "parser_shell_cmd.h"

#include "../list/parser_list.h"
#include "if/parser_if.h"
#include "loops/parser_for.h"
#include "loops/parser_loops.h"

enum parser_status parse_brackets(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    enum token_type type = token->type == TOKEN_CURLY_OPEN
        ? TOKEN_CURLY_CLOSE
        : TOKEN_BRACKET_CLOSE;

    lexer_pop(lexer);
    token_free(token);

    enum parser_status status = parse_compound_list(ast, lexer);
    if (status != PARSER_OK)
        return status;

    token = lexer_peek(lexer);
    if (token->type != type)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    lexer_pop(lexer);
    if (type == TOKEN_BRACKET_CLOSE)
    {
        struct ast *sub = init_ast(AST_SUBSHELL);
        sub->data.ast_sub.list = *ast;
        *ast = sub;
    }

    return PARSER_OK;
}

enum parser_status parse_shell_command(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_CURLY_OPEN || token->type == TOKEN_BRACKET_OPEN)
    {
        token_free(token);
        return parse_brackets(ast, lexer);
    }

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

    if (token->type == TOKEN_FOR)
    {
        token_free(token);
        return parse_for(ast, lexer);
    }

    token_free(token);
    return PARSER_UNKNOWN_TOKEN;
}
