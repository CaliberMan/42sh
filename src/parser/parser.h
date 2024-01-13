//#ifndef PARSER_H
//#define PARSER_H
#pragma once

#include "../ast/ast.h"
#include "../lexer/lexer.h"

enum parser_status
{
    PARSER_OK = 0,
    PARSER_UNKNOWN_TOKEN,
    PARSER_ERROR,
};

enum parser_status parse_input(struct ast **ast, struct lexer *lexer);

//#endif /* ! PARSER_H */
