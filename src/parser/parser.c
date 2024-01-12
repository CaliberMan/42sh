#include "parser.h"

#include <stddef.h>
#include <string.h>

// Prototypes of the functions used
enum parser_status parse_input(struct ast **ast, struct lexer *lexer);
enum parser_status parse_list(struct ast **ast, struct lexer *lexer);
enum parser_status parse_and_or(struct ast **ast, struct lexer *lexer);
enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer);
enum parser_status parse_command(struct ast **ast, struct lexer *lexer);
enum parser_status parse_simple_command(struct ast **ast, struct lexer *lexer);
enum parser_status parse_element(struct ast **ast, struct lexer *lexer);
enum parser_status parse_shell_command(struct ast **ast, struct lexer *lexer);
enum parser_status parse_rule_if_elif(struct ast **ast, struct lexer *lexer,
                                      enum token_type type);
enum parser_status parse_rule_if(struct ast **ast, struct lexer *lexer);
enum parser_status parse_else_clause(struct ast **ast, struct lexer *lexer);
enum parser_status parse_compound_list(struct ast **ast, struct lexer *lexer);
enum parser_status parse_compound_list_rep(struct ast **ast, struct lexer *lexer);

/**
 * @brief Parse a list, \n or nothing
 *
 * input =     list \n
 *          |  list EOF
 *          |  \n
 *          |  EOF
 *          ;
 */
enum parser_status parse_input(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_NEWLINE || token->type == TOKEN_EOF)
        return PARSER_OK;

    enum parser_status status = parse_list(ast, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    token = lexer_peek(lexer);
    if (token->type != TOKEN_NEWLINE &&token->type != TOKEN_EOF
            || lexer->prev_token->type == TOKEN_COLON)
        return PARSER_ERROR;

    return PARSER_OK;
}

/**
 * @brief Parse a list
 *
 * list =     and_or { ';' and_or } [ ';' ];
 */
enum parser_status parse_list(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = parse_and_or(ast, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    struct token *token = lexer_peek(lexer);
    while (token->type == TOKEN_COLON)
    {
        lexer_pop(lexer);

        struct ast *next;
        status = parse_and_or(&next, lexer);

        // means the next token is not in and_or rule
        if (status == PARSER_UNKNOWN_TOKEN)
            return PARSER_OK;
        if (status == PARSER_ERROR)
            return PARSER_ERROR;

        // add the next node to the ast
        struct ast *node = (*ast)->next;
        next->next = node;
        (*ast)->next = next;

        token = lexer_peek(lexer);
    }

    return PARSER_OK;
}

/**
 * @brief Parse and_or expressions
 *
 * and_or =     pipeline;
 */
enum parser_status parse_and_or(struct ast **ast, struct lexer *lexer)
{
    return parse_pipeline(ast, lexer);
}

/**
 * @brief Parse pipeline expressions
 *
 * pipeline =   command;
 */
enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer)
{
    return parse_command(ast, lexer);
}

/**
 * @brief Parse command expressions
 *
 * command =    simple_command
 *           |  shell_command
 *           ;
 */
enum parser_status parse_command(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_WORD)
        return parse_simple_command(ast, lexer);

    return parse_shell_command(ast, lexer);
}

/**
 * @brief Parse simple_command expressions
 *
 * simple_command = WORD { element };
 */
enum parser_status parse_simple_command(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
        return PARSER_UNKNOWN_TOKEN;

    struct ast *command_node = init_ast(AST_COMMAND);
    command_node->command[0] = token->data;

    command_node->left = *ast;

    lexer_pop(lexer);
    int index = 1;

    while (1)
    {
        struct ast *next_word;
        enum parser_status status = parse_element(&next_word, lexer);
        if (status == PARSER_UNKNOWN_TOKEN)
            return PARSER_OK;
        if (status == PARSER_ERROR)
            return PARSER_ERROR;

        command_node->command[index++] = next_word->command[0];
    }

    *ast = command_node;
    return PARSER_OK;
}

/**
 * @brief Parse element expressions
 *
 * element = WORD;
 */
enum parser_status parse_element(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
        return PARSER_UNKNOWN_TOKEN;

    struct ast *node = init_ast(AST_COMMAND);
    node->command[0] = token->data;
    *ast = node;

    return PARSER_OK;
}

/**
 * @brief Parse shell_command expressions
 *
 * shell_command = rule_if;
 */
enum parser_status parse_shell_command(struct ast **ast, struct lexer *lexer)
{
    return parse_rule_if(ast, lexer);
}

/**
 * @brief Parses if and elif rules since they are very similar. Not in the
 * grammer
 *
 * rule_if_elif = 'if' compound_list 'then' compound_list [else_clause];
 */
enum parser_status parse_rule_if_elif(struct ast **ast, struct lexer *lexer,
                                      enum token_type type)
{
    // check for IF
    struct token *token = lexer_peek(lexer);
    if (token->type != type)
        return PARSER_UNKNOWN_TOKEN;

    lexer_pop(lexer);

    struct ast *if_ast = init_ast(AST_IF);

    // check for the CONDITION
    struct ast *cond;
    enum parser_status status = parse_compound_list(&cond, lexer);
    if (status != PARSER_OK)
        return PARSER_ERROR;

    if_ast->op_ast = cond;

    // check if true (then)
    token = lexer_peek(lexer);
    if (token->type != TOKEN_THEN)
        return PARSER_ERROR;

    lexer_pop(lexer);

    struct ast *if_true;
    status = parse_compound_list(&if_true, lexer);
    if (status != PARSER_OK)
        return PARSER_ERROR;

    cond->left = if_true;

    // check if false (else)
    struct ast *if_false;
    status = parse_else_clause(&if_false, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    if (status == PARSER_OK)
        cond->right = if_false;

    *ast = if_ast;
    return PARSER_OK;
}

/**
 * @brief Parse rule_if expressions
 *
 * rule_if = 'if' compound_list 'then' compound_list [else_clause] 'fi';
 */
enum parser_status parse_rule_if(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = parse_rule_if_elif(ast, lexer, TOKEN_IF);
    if (status != PARSER_OK)
        return status;
    
    struct token *token = lexer_peek(lexer);
    // check for FI
    if (token->type != TOKEN_FI)
        return PARSER_ERROR;

    lexer_pop(lexer);
    return PARSER_OK;
}

/**
 * @brief Parse else_clause expressions
 *
 * else_clause =  'else' compound_list
 *              | 'elif' compound_list 'then' compound_list [else_clause]
 *              ;
 */
enum parser_status parse_else_clause(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_ELSE || token->type == TOKEN_ELIF)
        return PARSER_UNKNOWN_TOKEN;

    lexer_pop(lexer);

    if (token->type == TOKEN_ELSE)
    {
        lexer_pop(lexer);
        struct ast *ast_else;
        enum parser_status status = parse_compound_list(&ast_else, lexer);

        *ast = ast_else;
    }
    else if (token->type == TOKEN_ELIF)
    {
        lexer_pop(lexer);
        struct ast *ast_elif;
        enum parser_status status =
            parse_rule_if_elif(&ast_elif, lexer, TOKEN_ELIF);
        if (status != PARSER_OK)
            return status;

        *ast = ast_elif;
    }

    return PARSER_OK;
}

/**
 * @brief Popping concecutive tokens of type type 
 * @param lexer The lexer
 * @param type Type to be popped
 */
void pop_duplicates(struct lexer *lexer, enum token_type type)
{
    struct token *token = lexer_peek(lexer);
    while (token->type == type)
    {
        lexer_pop(lexer);
        token = lexer_peek(lexer);
    }
}

/**
 * @brief Parse compound_list expressions
 *
 * compound_list = {'\n'} and_or compound_list_rep [';'] {'\n'}
 *               ;
 */
enum parser_status parse_compound_list(struct ast **ast, struct lexer *lexer)
{
    // the NEWLINES
    pop_duplicates(lexer, TOKEN_NEWLINE);

    // the AND_OR
    struct ast *and_or_ast;
    enum parser_status status = parse_and_or(&and_or_ast, lexer);
    if (status != PARSER_OK)
       return PARSER_ERROR;

    *ast = and_or_ast;

    // the COMMAND_LIST_REP
    status = parse_compound_list_rep(ast, lexer);
    if (status != PARSER_OK)
        return status;

    // the ';'
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_COLON)
        lexer_pop(lexer);

    // the NEWLINES
    pop_duplicates(lexer, TOKEN_NEWLINE);
    return PARSER_OK;
}

/**
 * @brief Parse compound_list_rep
 *
 * compound_list_rep = { ( ';' | '\n' ) {'\n'} and_or }
 *                     ;
 */
enum parser_status parse_compound_list_rep(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = PARSER_OK;
    struct token *token;
    while (status == PARSER_OK)
    {
        // the (';' | '\n')
        token = lexer_peek(lexer);

        if (token->type == TOKEN_COLON || token->type == TOKEN_NEWLINE)
            lexer_pop(lexer);

        // the {'\n'}
        pop_duplicates(lexer, TOKEN_NEWLINE);

        // the ast_node
        struct ast *node;
        status = parse_and_or(&node, lexer);
        if (status == PARSER_ERROR)
            return PARSER_ERROR;

        if (status == PARSER_UNKNOWN_TOKEN)
            return PARSER_OK;
        
        struct ast *tmp = (*ast)->next;
        node->next = tmp;
        (*ast)->next = node;
    }

    return PARSER_OK;
}
