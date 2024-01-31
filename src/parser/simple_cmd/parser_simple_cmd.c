#include "parser_simple_cmd.h"

#include <stdlib.h>
#include <string.h>

#include "../list/parser_list.h"

enum parser_status get_word(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
    {
        token_free(token);
        return PARSER_UNKNOWN_TOKEN;
    }

    struct ast *cmd_node = init_ast(AST_CMD);
    struct ast_cmd *cmd = &cmd_node->data.ast_cmd;

    cmd->words[0] = calloc(token->len + 1, sizeof(char));
    cmd->words[0] = strcpy(cmd->words[0], token->data);

    *ast = cmd_node;

    lexer_pop(lexer);
    token_free(token);
    size_t index = 1;

    while (1)
    {
        struct ast *next_word;
        enum parser_status status = parse_element(&next_word, lexer);
        if (index == cmd->capacity)
            realloc_words(cmd);

        if (status == PARSER_UNKNOWN_TOKEN)
            return PARSER_OK;
        if (status == PARSER_ERROR)
            return PARSER_ERROR;

        // do different things depending on the type
        if (next_word->type == AST_REDIR)
        {
            next_word->data.ast_redir.left = *ast;
            *ast = next_word;
            index = 0;
        }
        else
        {
            struct ast_cmd *next_cmd = &next_word->data.ast_cmd;
            cmd->words[index] =
                calloc(strlen(next_cmd->words[0]) + 1, sizeof(char));
            cmd->words[index] = strcpy(cmd->words[index], next_cmd->words[0]);
            free_ast(next_word);
            index++;
        }
    }

    return PARSER_OK;
}

enum parser_status parse_simple_command(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = parse_prefix(ast, lexer);
    if (status == PARSER_ERROR)
        return status;

    // no prefix
    if (status == PARSER_UNKNOWN_TOKEN)
    {
        // no prefix and no word
        struct token *token = lexer_peek(lexer);
        if (token->type != TOKEN_WORD)
        {
            token_free(token);
            return PARSER_ERROR;
        }

        token_free(token);
        return get_word(ast, lexer);
    }

    // yes prefix
    if ((*ast)->type == AST_VARIABLE)
        return PARSER_OK;

    while (1)
    {
        struct ast *node;
        status = parse_prefix(&node, lexer);
        if (status == PARSER_UNKNOWN_TOKEN)
            break;
        if (status == PARSER_ERROR)
            return status;

        // add the ast
        node->data.ast_redir.left = *ast;
        *ast = node;
    }

    // check if you get word
    struct ast *node;
    status = get_word(&node, lexer);

    if (status == PARSER_UNKNOWN_TOKEN)
        return PARSER_OK;
    if (status == PARSER_OK)
    {
        // take care of the tree
        node->data.ast_redir.left = *ast;
        *ast = node;
        return PARSER_OK;
    }
    else
        return status;
}

enum parser_status parse_prefix(struct ast **ast, struct lexer *lexer)
{
    // check if its an =
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_ASSIGN)
    {
        lexer_pop(lexer);
        struct ast *variable = init_ast(AST_VARIABLE);
        *ast = variable;

        token_free(token);
        return PARSER_OK;
    }

    token_free(token);
    return parse_redirection(ast, lexer);
}

enum parser_status parse_element(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = parse_redirection(ast, lexer);
    if (status != PARSER_UNKNOWN_TOKEN)
        return status;

    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
    {
        token_free(token);
        return PARSER_UNKNOWN_TOKEN;
    }

    struct ast *cmd_node = init_ast(AST_CMD);
    struct ast_cmd *cmd = &cmd_node->data.ast_cmd;

    cmd->words[0] = calloc(token->len + 1, sizeof(char));
    cmd->words[0] = strcpy(cmd->words[0], token->data);
    *ast = cmd_node;

    lexer_pop(lexer);
    token_free(token);
    return PARSER_OK;
}
