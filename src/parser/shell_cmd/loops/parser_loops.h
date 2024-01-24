#ifndef PARSER_LOOPS_H
#define PARSER_LOOPS_H

#include "../../utils.h"

enum parser_status parse_while_until(struct ast **ast, struct lexer *lexer, enum loop_type type);

#endif /* ! PARSER_LOOPS_H */
