#ifndef INPUT_H
#define INPUT_H

#include "../list/parser_list.h"
#include "../utils.h"

/**
 * @brief Parse a list, \n or nothing
 *
 * input =     list \n
 *          |  list EOF
 *          |  \n
 *          |  EOF
 *          ;
 */
enum parser_status parse_input(struct ast **ast, struct lexer *lexer);

#endif /* ! INPUT_H */
