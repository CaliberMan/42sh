#include "parser_case.h"

enum parser_status parse_item(struct ast **ast, struct lexer *lexer)
{
    if (!ast || !lexer)
        return PARSER_ERROR;

    return PARSER_UNKNOWN_TOKEN;
}

enum parser_status parse_clause(struct ast **ast, struct lexer *lexer)
{
    if (!ast || !lexer)
        return PARSER_ERROR;

    return PARSER_UNKNOWN_TOKEN;
}

enum parser_status parse_case(struct ast **ast, struct lexer *lexer)
{
    if (!ast || !lexer)
        return PARSER_ERROR;

    return PARSER_UNKNOWN_TOKEN;
}
