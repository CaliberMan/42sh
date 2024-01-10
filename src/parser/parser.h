#ifndef PARSER_H
#define PARSER_H

enum parser_status
{
    PARSER_OK,
    PARSER_ERROR
};

/**
 * @brief Parse a list, \n or nothing
 * 
 * input =     list \n
 *          |  list EOF
 *          |  \n
 *          |  EOF ;
 */
enum parser_status parse_input(struct ast **ast, struct lexer *lexer);

/**
 * @brief Parse a list
 * 
 * list =     and_or;
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
 * command =   simple_command;
 */
enum parser_status parse_command(struct ast **ast, struct lexer *lexer);

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

#endif /* ! PARSER_H */
