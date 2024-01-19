#ifndef PARSER_LIST_H
#define PARSER_LIST_H

#include "../shell_cmd/parser_shell_cmd.h"
#include "../simple_cmd/parser_simple_cmd.h"
#include "../utils.h"

/**
 * @brief Parse a list
 *
 * list =     and_or { ';' and_or } [ ';' ];
 */
enum parser_status parse_list(struct ast **ast, struct lexer *lexer);

/**
 * @brief Parse and_or expressions
 *
 * and_or =     pipeline;
 */
enum parser_status parse_and_or(struct ast **ast, struct lexer *lexer);

/**
 * @brief Parse pipeline expressions
 *
 * pipeline =   command;
 */
enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer);

/**
 * @brief Parse command expressions
 *
 * command =    simple_command
 *           |  shell_command
 *           ;
 */
enum parser_status parse_command(struct ast **ast, struct lexer *lexer);

/**
 * @brief Parse compound_list expressions
 *
 * compound_list = {'\n'} and_or compound_list_rep [';'] {'\n'}
 *               ;
 */
enum parser_status parse_compound_list(struct ast **ast, struct lexer *lexer);

/**
 * @brief Parse compound_list_rep
 *
 * compound_list_rep = { ( ';' | '\n' ) {'\n'} and_or }
 *                     ;
 */
enum parser_status parse_compound_list_rep(struct ast **ast,
                                           struct lexer *lexer);

/**
 * @brief Popping concecutive tokens of type type
 * @param lexer The lexer
 * @param type Type to be popped
 */
void pop_duplicates(struct lexer *lexer, enum token_type type);

#endif /* ! PARSER_LIST_H */
