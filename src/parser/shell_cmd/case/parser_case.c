#include "parser_case.h"

#include <string.h>

#include "../../list/parser_list.h"

enum parser_status parse_item(struct ast **ast, struct lexer *lexer)
{
    struct ast *pattern = init_ast(AST_PATTERN);
    struct ast *list = init_ast(AST_CMD);

    pattern->data.ast_pattern.pattern = list;
    *ast = pattern;
    size_t index = 0;

    struct token *token = lexer_peek(lexer);
    while (1)
    {
        if (token->type == TOKEN_WORD)
        {
            list->data.ast_cmd.words[index] =
                calloc(token->len + 1, sizeof(char));
            list->data.ast_cmd.words[index] =
                strcpy(list->data.ast_cmd.words[index], token->data);
            lexer_pop(lexer);
            index++;
        }
        else if (token->type == TOKEN_PIPE)
            lexer_pop(lexer);
        else if (token->type == TOKEN_ESAC)
        {
            token_free(token);
            return PARSER_OK;
        }
        else if (token->type == TOKEN_BRACKET_CLOSE)
            break;
        else
        {
            token_free(token);
            return PARSER_ERROR;
        }

        token_free(token);
        token = lexer_peek(lexer);
    }

    token_free(token);
    lexer_pop(lexer);
    pop_duplicates(lexer, TOKEN_NEWLINE);

    struct ast *statement;
    enum parser_status status = parse_compound_list(&statement, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;
    if (status == PARSER_UNKNOWN_TOKEN)
        return PARSER_OK;

    pattern->data.ast_pattern.statement = statement;
    return PARSER_OK;
}

enum parser_status parse_clause(struct ast **ast, struct lexer *lexer)
{
    struct ast *ast_list = init_ast(AST_LIST);
    struct ast_list *list = &ast_list->data.ast_list;
    (*ast)->data.ast_case.cases_list = ast_list;

    struct ast *item;
    enum parser_status status = parse_item(&item, lexer);
    if (status != PARSER_OK)
        return PARSER_ERROR;

    size_t index = 0;
    add_ast(list, item, &index);
    struct token *token = lexer_peek(lexer);

    while (1)
    {
        if (token->type != TOKEN_DOUBLE_COLON)
            break;

        token_free(token);
        lexer_pop(lexer);
        pop_duplicates(lexer, TOKEN_NEWLINE);

        struct ast *item;
        status = parse_item(&item, lexer);
        add_ast(list, item, &index);

        if (status == PARSER_UNKNOWN_TOKEN)
            return PARSER_OK;
        if (status == PARSER_ERROR)
            return status;

        token = lexer_peek(lexer);
    }

    token_free(token);
    pop_duplicates(lexer, TOKEN_NEWLINE);
    return PARSER_OK;
}

enum parser_status parse_case(struct ast **ast, struct lexer *lexer)
{
    lexer_pop(lexer);
    struct ast *rule_case = init_ast(AST_CASE);
    struct ast_case *case_ast = &rule_case->data.ast_case;
    *ast = rule_case;

    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    case_ast->expr = calloc(token->len + 1, sizeof(char));
    case_ast->expr = strcpy(case_ast->expr, token->data);

    token_free(token);
    lexer_pop(lexer);

    pop_duplicates(lexer, TOKEN_NEWLINE);
    token = lexer_peek(lexer);
    if (token->type != TOKEN_IN)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    token_free(token);
    lexer_pop(lexer);
    pop_duplicates(lexer, TOKEN_NEWLINE);

    enum parser_status status = parse_clause(ast, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    token = lexer_peek(lexer);
    if (token->type != TOKEN_ESAC)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    token_free(token);
    lexer_pop(lexer);
    return PARSER_OK;
}
