#include "parser_if.h"

#include "../../list/parser_list.h"

enum parser_status parse_rule_if_elif(struct ast **ast, struct lexer *lexer,
                                      enum token_type type)
{
    // check for IF
    struct token *token = lexer_peek(lexer);
    if (token->type != type)
    {
        token_free(token);
        return PARSER_UNKNOWN_TOKEN;
    }

    lexer_pop(lexer);
    token_free(token);

    struct ast *if_ast = init_ast();

    // check for the CONDITION
    struct ast *cond;
    enum parser_status status = parse_compound_list(&cond, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    if_ast->data.ast_if.cond = cond;

    // check if true (then)
    token = lexer_peek(lexer);
    if (token->type != TOKEN_THEN)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    lexer_pop(lexer);
    token_free(token);

    struct ast *if_true;
    status = parse_compound_list(&if_true, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    if_ast->data.ast_if.then_body = if_true;

    // check if false (else)
    struct ast *if_false;
    status = parse_else_clause(&if_false, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    if (status == PARSER_OK)
        if_ast->data.ast_if.else_body = if_false;

    *ast = if_ast;
    return PARSER_OK;
}

enum parser_status parse_rule_if(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = parse_rule_if_elif(ast, lexer, TOKEN_IF);
    if (status != PARSER_OK)
        return status;

    struct token *token = lexer_peek(lexer);
    // check for FI
    if (token->type != TOKEN_FI)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    lexer_pop(lexer);
    token_free(token);
    return PARSER_OK;
}

enum parser_status parse_else_clause(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_ELSE && token->type != TOKEN_ELIF)
    {
        token_free(token);
        return PARSER_UNKNOWN_TOKEN;
    }

    lexer_pop(lexer);

    if (token->type == TOKEN_ELSE)
    {
        struct ast *ast_else;
        enum parser_status status = parse_compound_list(&ast_else, lexer);
        if (status == PARSER_ERROR)
            return PARSER_ERROR;

        *ast = ast_else;
    }
    else if (token->type == TOKEN_ELIF)
    {
        struct ast *ast_elif;
        enum parser_status status =
            parse_rule_if_elif(&ast_elif, lexer, TOKEN_ELIF);
        if (status == PARSER_ERROR)
            return status;

        *ast = ast_elif;
    }

    token_free(token);
    return PARSER_OK;
}
