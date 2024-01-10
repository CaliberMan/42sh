#pragma once
//#ifndef PARSER_H
//#define PARSER_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"

enum parser_status
{
    PARSER_OK = 0,
    PARSER_ERROR,
};

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

/**
 * @brief Parse shell_command expressions
 * 
 * shell_command = rule_if;
 */
enum parser_status parse_shell_command(struct ast **ast, struct lexer *lexer);

/**
 * @brief Parse rule_if expressions
 * 
 * rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi';
 */
enum parser_status parse_rule_if(struct ast **ast, struct lexer *lexer);

/**
 * @brief Parse else_clause expressions
 * 
 * else_clause =  'else' compound_list
 *              | 'elif' compound_list 'then' compound_list [else_clause]
 *              ;
 */
enum parser_status parse_else_clause(struct ast **ast, struct lexer *lexer);

/**
 * @brief Parse command_list expressions
 * 
 * command_list = {'\n'} and_or { ( ';' | '\n' ) {'\n' and_or } [';'] {'\n'}
 *               ;
 */
enum parser_status parse_command_list(struct ast **ast, struct lexer *lexer);

//#endif /* ! PARSER_H */
