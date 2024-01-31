#ifndef PARSER_IF_H
#define PARSER_IF_H

#include "../../utils.h"

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

#endif /* ! PARSER_IF_H */
