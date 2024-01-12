#pragma once

#include "ast.h"
#include "lexer.h"

enum parser_status
{
    PARSER_OK = 0,
    PARSER_UNKNOWN_TOKEN,
    PARSER_ERROR,
};

enum parser_status parse_input(struct ast **ast, struct lexer *lexer);
