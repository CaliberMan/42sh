#ifndef UTILS_H
#define UTILS_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"

enum parser_status
{
    PARSER_OK = 0,
    PARSER_UNKNOWN_TOKEN,
    PARSER_ERROR,
};

#endif /* ! UTILS_H */
