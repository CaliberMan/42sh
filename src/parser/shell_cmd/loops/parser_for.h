#ifndef PARSER_FOR_H
#define PARSER_FOR_H

#include "../parser_shell_cmd.h"

enum parser_status parse_for(struct ast **ast, struct lexer *lexer);

#endif /* ! PARSER_FOR_H */
