#include "parser_for.h"

#include <stdlib.h>
#include <string.h>

#include "../../list/parser_list.h"

void copy_word(struct token *token, struct ast *dest, int index)
{
    if (dest->type == AST_CMD)
    {
        dest->data.ast_cmd.words[index] = calloc(token->len + 1, sizeof(char));
        dest->data.ast_cmd.words[index] =
            strcpy(dest->data.ast_cmd.words[index], token->data);
    }
    else if (dest->type == AST_LOOP)
    {
        dest->data.ast_loop.var_name = calloc(token->len + 1, sizeof(char));
        dest->data.ast_loop.var_name =
            strcpy(dest->data.ast_loop.var_name, token->data);
    }
}

enum parser_status rule1(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_COLON)
    {
        lexer_pop(lexer);
        token_free(token);
        return PARSER_OK;
    }

    token_free(token);
    return PARSER_UNKNOWN_TOKEN;
}

enum parser_status rule2(struct ast **ast, struct lexer *lexer)
{
    pop_duplicates(lexer, TOKEN_NEWLINE);

    struct token *token = lexer_peek(lexer);
    lexer_pop(lexer);
    if (token->type != TOKEN_IN)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    token_free(token);
    struct ast *word_list = init_ast(AST_CMD);
    *ast = word_list;
    int index = 0;

    token = lexer_peek(lexer);
    while (token->type == TOKEN_WORD)
    {
        lexer_pop(lexer);
        copy_word(token, word_list, index);

        token_free(token);
        lexer_peek(lexer);
    }

    if (token->type != TOKEN_COLON || token->type != TOKEN_NEWLINE)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    lexer_pop(lexer);
    token_free(token);

    return PARSER_OK;
}

enum parser_status parse_for(struct ast **ast, struct lexer *lexer)
{
    lexer_pop(lexer);
    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    lexer_pop(lexer);

    struct ast *for_loop = init_ast(AST_LOOP);
    for_loop->data.ast_loop.type = FOR_LOOP;

    // got variable name
    copy_word(token, for_loop, -1);
    *ast = for_loop;

    token_free(token);

    // check rule1
    enum parser_status status = rule1(lexer);
    if (status != PARSER_OK)
    {
        // check rule2
        struct ast *cond;
        status = rule2(&cond, lexer);
        if (status != PARSER_OK)
            return PARSER_ERROR;

        for_loop->data.ast_loop.cond = cond;
    }

    pop_duplicates(lexer, TOKEN_NEWLINE);

    token = lexer_peek(lexer);
    if (token->type != TOKEN_DO)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    token_free(token);
    lexer_pop(lexer);

    struct ast *then_body;
    status = parse_compound_list(&then_body, lexer);
    if (status != PARSER_OK)
        return PARSER_ERROR;

    for_loop->data.ast_loop.then_body = then_body;
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
