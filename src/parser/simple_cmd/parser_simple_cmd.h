#ifndef PARSER_SIMPLE_CMD_H
#define PARSER_SIMPLE_CMD_H

#include "../utils.h"

/**
 * @brief Parse simple_command expressions
 *
 * simple_command = WORD { element };
 */
enum parser_status parse_simple_command(struct ast **ast, struct lexer *lexer);

/**
 * @brief Parse element expressions
 *
 * element = WORD;
 */
enum parser_status parse_element(struct ast **ast, struct lexer *lexer);

#endif /* ! PARSER_SIMPLE_CMD_H */
