#ifndef PARSER_SHELL_CMD_H
#define PARSER_SHELL_CMD_H

#include "../utils.h"

/**
 * @brief Parse shell_command expressions
 *
 * shell_command = rule_if;
 */
enum parser_status parse_shell_command(struct ast **ast, struct lexer *lexer);

#endif /* ! PARSER_SHELL_CMD_H */
