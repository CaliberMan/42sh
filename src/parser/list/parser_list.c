#include "parser_list.h"

#include <ctype.h>

enum parser_status parse_list(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = parse_and_or(ast, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    struct ast *iterator = *ast;

    struct token *token = lexer_peek(lexer);
    while (token->type == TOKEN_COLON || token->type == TOKEN_NEWLINE)
    {
        lexer_pop(lexer);

        struct ast *next;
        status = parse_and_or(&next, lexer);

        // means the next token is not in and_or rule
        if (status == PARSER_UNKNOWN_TOKEN)
        {
            token_free(token);
            return PARSER_OK;
        }
        if (status == PARSER_ERROR)
        {
            token_free(token);
            return PARSER_ERROR;
        }

        // add the next node to the ast
        iterator->next = next;
        iterator = iterator->next;

        token_free(token);
        token = lexer_peek(lexer);
    }

    token_free(token);
    return PARSER_OK;
}

enum parser_status parse_and_or(struct ast **ast, struct lexer *lexer)
{
    return parse_pipeline(ast, lexer);
}

enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = parse_command(ast, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    if (status == PARSER_UNKNOWN_TOKEN)
        return PARSER_UNKNOWN_TOKEN;

    int num_args = 0;
    while (1)
    {
        struct token *token = lexer_peek(lexer);
        if (token->type != TOKEN_PIPE)
        {
            token_free(token);
            return PARSER_OK;
        }

        lexer_pop(lexer);
        token_free(token);
        pop_duplicates(lexer, TOKEN_NEWLINE);

        num_args++;
        struct ast *pipe_ast = init_ast(AST_PIPE);
        pipe_ast->data.ast_pipe.left_arg = *ast;
        *ast = pipe_ast;

        struct ast *right_arg;
        status = parse_command(&right_arg, lexer);
        if (status == PARSER_ERROR)
            return status;
        if (status == PARSER_UNKNOWN_TOKEN && num_args % 2 != 0)
            return PARSER_ERROR;
        if (status == PARSER_UNKNOWN_TOKEN && num_args % 2 == 0)
            return PARSER_OK;

        pipe_ast->data.ast_pipe.right_arg = right_arg;
        num_args++;
    }

    return PARSER_OK;
}

enum parser_status parse_command(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_WORD)
    {
        enum parser_status status = parse_simple_command(ast, lexer);
        if (status != PARSER_UNKNOWN_TOKEN)
        {
            token_free(token);
            return status;
        }
    }

    token_free(token);
    enum parser_status status = parse_shell_command(ast, lexer);
    if (status != PARSER_OK)
        return status;

    while (1)
    {
        struct ast *redirect_node;
        status = parse_redirection(&redirect_node, lexer); 
        if (status == PARSER_ERROR)
            return PARSER_ERROR;
        if (status == PARSER_UNKNOWN_TOKEN)
            return PARSER_OK;

        *ast = redirect_node;
    }

    return PARSER_OK;
}

enum parser_status parse_redirection(struct ast **ast, struct lexer *lexer)
{
    // CHECK IONUMBER 0 or 1 count
    struct ast *ast_redir = init_ast(AST_REDIR);
    *ast = ast_redir;

    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_WORD)
    {
        for (int i = 0; i < token->len; i++)
        {
            if (!isdigit(token->data[i]))
                return PARSER_ERROR;
        }
    }

    token_free(token);
    lexer_pop(lexer);

    // CHECK IF TOKEN IS REDIRECT
    struct token *token2 = lexer_peek(lexer);

    if (token2->type != TOKEN_REDIR)
	    return PARSER_ERROR;

    token_free(token);
    lexer_pop(lexer);

    // CHECK IF LAST TOKEN IS WORD, WE DO NOT CARE WHATS AFTER
    token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
	    return PARSER_ERROR;

    return PARSER_OK;
}

enum parser_status parse_compound_list(struct ast **ast, struct lexer *lexer)
{
    // the NEWLINES
    pop_duplicates(lexer, TOKEN_NEWLINE);

    // the AND_OR
    struct ast *and_or_ast;
    enum parser_status status = parse_and_or(&and_or_ast, lexer);
    if (status != PARSER_OK)
        return PARSER_ERROR;

    *ast = and_or_ast;

    // the COMMAND_LIST_REP
    status = parse_compound_list_rep(ast, lexer);
    if (status == PARSER_UNKNOWN_TOKEN)
        return PARSER_OK;
    if (status != PARSER_OK)
        return status;

    // the ';'
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_COLON)
        lexer_pop(lexer);

    // the NEWLINES
    pop_duplicates(lexer, TOKEN_NEWLINE);

    token_free(token);
    return PARSER_OK;
}

enum parser_status parse_compound_list_rep(struct ast **ast,
                                           struct lexer *lexer)
{
    enum parser_status status = PARSER_OK;
    struct token *token;
    struct ast *iterator = *ast;
    while (status == PARSER_OK)
    {
        // the (';' | '\n')
        token = lexer_peek(lexer);

        if (token->type == TOKEN_COLON || token->type == TOKEN_NEWLINE)
            lexer_pop(lexer);

        token_free(token);

        // the {'\n'}
        pop_duplicates(lexer, TOKEN_NEWLINE);

        // the ast_node
        struct ast *node;
        status = parse_and_or(&node, lexer);
        if (status != PARSER_OK)
            return status;
        //if (status == PARSER_ERROR)
        //    return PARSER_ERROR;

        //if (status == PARSER_UNKNOWN_TOKEN)
        //    return PARSER_OK;

        iterator->next = node;
        iterator = iterator->next;
    }

    return PARSER_OK;
}

void pop_duplicates(struct lexer *lexer, enum token_type type)
{
    struct token *token = lexer_peek(lexer);
    while (token->type == type)
    {
        lexer_pop(lexer);
        token_free(token);
        token = lexer_peek(lexer);
    }

    token_free(token);
}
