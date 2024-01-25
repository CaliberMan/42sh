#include "parser_list.h"

#include <ctype.h>
#include <string.h>

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
    enum parser_status status = parse_pipeline(ast, lexer);
    if (status != PARSER_OK)
        return status;

    while (1)
    {
        struct token *token = lexer_peek(lexer);
        if (token->type != TOKEN_AND && token->type != TOKEN_OR)
        {
            token_free(token);
            return PARSER_OK;
        }

        enum operator_type type = token->type == TOKEN_OR ? OP_OR : OP_AND;
        token_free(token);
        lexer_pop(lexer);

        struct ast *op_node = init_ast(AST_OPERATOR);
        op_node->data.ast_operator.type = type;
        op_node->data.ast_operator.left = *ast;
        *ast = op_node;

        pop_duplicates(lexer, TOKEN_NEWLINE);

        struct ast *right;
        status = parse_pipeline(&right, lexer);

        if (status != PARSER_OK)
            return PARSER_ERROR;

        op_node->data.ast_operator.right = right;
    }

    return PARSER_OK;
}

enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    enum parser_status status = PARSER_OK;
    if (token->type == TOKEN_NOT)
    {
        struct ast *not_ast = init_ast(AST_NOT);
        struct ast *new_node;

        lexer_pop(lexer);

        status = parse_command(&new_node, lexer);
        if (status == PARSER_OK)
        {
            not_ast->data.ast_not.child = new_node;
            *ast = not_ast;
        }
    }
    else
        status = parse_command(ast, lexer);

    token_free(token);

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

        redirect_node->data.ast_redir.left = *ast;
        *ast = redirect_node;
    }

    return PARSER_OK;
}

enum redir_type parse_redir_type(char *str)
{
    if (strcmp(str, ">>"))
        return STD_OUT_END;
    if (strcmp(str, ">|"))
        return STD_RIGHT_ARROW_PIPE;
    if (strcmp(str, ">"))
        return STD_OUT;
    if (strcmp(str, "<"))
        return STD_IN;
    if (strcmp(str, "<>"))
        return STD_IN_OUT;
    if (strcmp(str, ">&"))
        return STD_ERR;
    return STD_LEFT_ARROW_AND;
}

enum parser_status parse_redirection(struct ast **ast, struct lexer *lexer)
{
    // CHECK IONUMBER 0 or 1 count
    int io = 0;
    int total = -1;

    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_WORD)
    {
        total = 0;
        for (int i = 0; i < token->len; i++)
        {
            if (!isdigit(token->data[i]))
            {
                token_free(token);
                return PARSER_UNKNOWN_TOKEN;
            }
            total *= 10;
            total += token->data[i] - '0';
        }
        io = 1;
        lexer_pop(lexer);
    }

    token_free(token);

    // CHECK IF TOKEN IS REDIRECT
    token = lexer_peek(lexer);

    if (token->type != TOKEN_REDIR)
    {
        token_free(token);
        if (io)
            return PARSER_ERROR;
        return PARSER_UNKNOWN_TOKEN;
    }

    struct ast *ast_redir = init_ast(AST_REDIR);
    *ast = ast_redir;

    (*ast)->data.ast_redir.ioNumber = total;
    (*ast)->data.ast_redir.type = parse_redir_type(token->data);

    token_free(token);
    lexer_pop(lexer);

    // CHECK IF LAST TOKEN IS WORD, WE DO NOT CARE WHATS AFTER
    token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    lexer_pop(lexer);
    struct ast *ast_file = init_ast(AST_FILE);
    ast_file->data.ast_file.filename = token->data;

    (*ast)->data.ast_redir.right = ast_file;

    free(token);
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
        // if (status == PARSER_ERROR)
        //     return PARSER_ERROR;

        // if (status == PARSER_UNKNOWN_TOKEN)
        //     return PARSER_OK;

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
