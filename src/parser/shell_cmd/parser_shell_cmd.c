#include "parser_shell_cmd.h"

#include "if/parser_if.h"

enum parser_status parse_shell_command(struct ast **ast, struct lexer *lexer)
{
    return parse_rule_if(ast, lexer);
}
