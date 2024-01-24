#include "parser_loops.h"

#include "../list/parser_list.h"

enum parser_status parse_while(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek();
    if (token->type != TOKEN_WHILE)
    {
        token_free(token);
	return PARSER_UNKNOWN_TOKEN;
    }
    struct ast *ast_while = init_ast(AST_LOOP);
    ast_while->data.ast_loop.type = WHILE_LOOP;

    struct ast *cond;
    enum parser_status ps = parse_compound_list(&cond, lexer);

    if (ps != PARSER_OK)
    {
	// FIX
	return PARSER_ERROR;
    }

    return PARSER_OK;
}

enum parser_status parse_until(struct ast **ast, struct lexer *lexer)
{
    return PARSER_OK;
}
