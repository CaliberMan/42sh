#include "parser_loops.h"

#include "../../list/parser_list.h"

enum parser_status parse_while_until(struct ast **ast, struct lexer *lexer, enum loop_type type)
{
    lexer_pop(lexer);

    struct ast *ast_while = init_ast(AST_LOOP);
    ast_while->data.ast_loop.type = type;
    *ast = ast_while;

    struct ast *cond;
    enum parser_status ps = parse_compound_list(&cond, lexer);

    if (ps == PARSER_ERROR)
        return ps;

    ast_while->data.ast_loop.cond = cond;

    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_DO)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    lexer_pop(lexer);

    struct ast *then_body;
    ps = parse_compound_list(&then_body, lexer);

    if (ps == PARSER_ERROR)
        return ps;

    ast_while->data.ast_loop.then_body = then_body;

    token = lexer_peek(lexer);
    if (token->type != TOKEN_DONE)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    lexer_pop(lexer);
    token_free(token);
    return PARSER_OK;
}
