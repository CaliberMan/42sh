#include "parser_simple_cmd.h"

#include <stdlib.h>
#include <string.h>

enum parser_status parse_simple_command(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
    {
        token_free(token);
        return PARSER_UNKNOWN_TOKEN;
    }

    struct ast *cmd_node = init_ast();
    struct ast_cmd *cmd = &cmd_node->data.ast_cmd;
    init_words(cmd);

    cmd->words[0] = calloc(token->len + 1, sizeof(char));
    cmd->words[0] = strcpy(cmd->words[0], token->data);

    *ast = cmd_node;

    lexer_pop(lexer);
    token_free(token);
    int index = 1;

    while (1)
    {
        struct ast *next_word;
        enum parser_status status = parse_element(&next_word, lexer);
        if (status == PARSER_UNKNOWN_TOKEN)
            return PARSER_OK;
        if (status == PARSER_ERROR)
            return PARSER_ERROR;

        struct ast_cmd *next_cmd = &next_word->data.ast_cmd;
        if (index == cmd->capacity)
            realloc_words(cmd);

        cmd->words[index] =
            calloc(strlen(next_cmd->words[0]) + 1, sizeof(char));
        cmd->words[index] = strcpy(cmd->words[index], next_cmd->words[0]);
        free_ast(next_word);
        index++;
    }

    return PARSER_OK;
}

enum parser_status parse_element(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
    {
        token_free(token);
        return PARSER_UNKNOWN_TOKEN;
    }

    struct ast *cmd_node = init_ast();
    struct ast_cmd *cmd = &cmd_node->data.ast_cmd;
    init_words(cmd);

    cmd->words[0] = calloc(token->len + 1, sizeof(char));
    cmd->words[0] = strcpy(cmd->words[0], token->data);
    *ast = cmd_node;

    lexer_pop(lexer);
    token_free(token);
    return PARSER_OK;
}
