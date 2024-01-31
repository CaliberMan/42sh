#include "parser_list.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

enum parser_status variable_list(struct ast **ast, struct lexer *lexer,
                                 size_t index)
{
    if (index == 0)
        return PARSER_ERROR;

    struct ast_list *ast_list = &(*ast)->data.ast_list;

    if (ast_list->list[index - 1]->type != AST_CMD
        && !ast_list->list[index - 1]->data.ast_cmd.words[1])
        return PARSER_ERROR;

    struct ast *assign;
    enum parser_status status = parse_and_or(&assign, lexer);
    if (status != PARSER_OK)
        return PARSER_ERROR;

    assign->data.ast_variable.name =
        calloc(strlen(ast_list->list[index - 1]->data.ast_cmd.words[0]) + 1,
               sizeof(char));
    assign->data.ast_variable.name =
        strcpy(assign->data.ast_variable.name,
               ast_list->list[index - 1]->data.ast_cmd.words[0]);
    free_ast(ast_list->list[index - 1]);

    // add it to the list
    ast_list->list[index - 1] = assign;

    struct ast *value;
    status = parse_and_or(&value, lexer);
    if (status != PARSER_OK)
        return PARSER_ERROR;

    assign->data.ast_variable.value = value;
    ast_list->nb_nodes++;

    return PARSER_OK;
}

enum parser_status parse_list(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = parse_and_or(ast, lexer);
    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    // create an ast list
    size_t index = 0;
    struct ast *ast_list = init_ast(AST_LIST);
    ast_list->data.ast_list.list[index++] = *ast;
    ast_list->data.ast_list.nb_nodes++;

    *ast = ast_list;

    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_ASSIGN)
    {
        token_free(token);
        if (variable_list(ast, lexer, index) != PARSER_OK)
            return PARSER_ERROR;
    }
    else
        token_free(token);

    token = lexer_peek(lexer);
    while (token->type == TOKEN_COLON || token->type == TOKEN_NEWLINE)
    {
        lexer_pop(lexer);
        token_free(token);

        struct ast *next;
        status = parse_and_or(&next, lexer);

        // means the next token is not in and_or rule
        if (status == PARSER_UNKNOWN_TOKEN)
            return PARSER_OK;
        if (status == PARSER_ERROR)
            return PARSER_ERROR;

        // increase the size if needed
        if (index == ast_list->data.ast_list.capacity)
            double_list_size(&ast_list->data.ast_list);

        ast_list->data.ast_list.list[index++] = next;
        ast_list->data.ast_list.nb_nodes++;

        token = lexer_peek(lexer);
        if (token->type == TOKEN_ASSIGN)
        {
            token_free(token);
            if (variable_list(ast, lexer, index) != PARSER_OK)
                return PARSER_ERROR;
        }
        else
            token_free(token);

        token = lexer_peek(lexer);
    }

    token_free(token);
    return PARSER_OK;
}

enum parser_status parse_and_or(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = parse_pipeline(ast, lexer);
    if (status != PARSER_OK)
        return status;

    while (1)
    {
        struct token *token = lexer_peek(lexer);
        if (token->type != TOKEN_AND && token->type != TOKEN_OR)
        {
            token_free(token);
            return PARSER_OK;
        }

        enum operator_type type = token->type == TOKEN_OR ? OP_OR : OP_AND;
        token_free(token);
        lexer_pop(lexer);

        struct ast *op_node = init_ast(AST_OPERATOR);
        op_node->data.ast_operator.type = type;
        op_node->data.ast_operator.left = *ast;
        *ast = op_node;

        pop_duplicates(lexer, TOKEN_NEWLINE);

        struct ast *right;
        status = parse_pipeline(&right, lexer);

        if (status != PARSER_OK)
            return PARSER_ERROR;

        op_node->data.ast_operator.right = right;
    }

    return PARSER_OK;
}

enum parser_status parse_pipeline(struct ast **ast, struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    enum parser_status status = PARSER_OK;
    if (token->type == TOKEN_NOT)
    {
        struct ast *not_ast = init_ast(AST_NOT);
        struct ast *new_node;

        lexer_pop(lexer);

        status = parse_command(&new_node, lexer);
        if (status == PARSER_OK)
        {
            not_ast->data.ast_not.child = new_node;
            *ast = not_ast;
        }
    }
    else
        status = parse_command(ast, lexer);

    token_free(token);

    if (status == PARSER_ERROR)
        return PARSER_ERROR;

    if (status == PARSER_UNKNOWN_TOKEN)
        return PARSER_UNKNOWN_TOKEN;

    int num_args = 0;
    while (1)
    {
        struct token *token = lexer_peek(lexer);
        if (token->type != TOKEN_PIPE)
        {
            token_free(token);
            return PARSER_OK;
        }

        lexer_pop(lexer);
        token_free(token);
        pop_duplicates(lexer, TOKEN_NEWLINE);

        num_args++;
        struct ast *pipe_ast = init_ast(AST_PIPE);
        pipe_ast->data.ast_pipe.left_arg = *ast;
        *ast = pipe_ast;

        struct ast *right_arg;
        status = parse_command(&right_arg, lexer);
        if (status == PARSER_ERROR)
            return status;
        if (status == PARSER_UNKNOWN_TOKEN && num_args % 2 != 0)
            return PARSER_ERROR;
        if (status == PARSER_UNKNOWN_TOKEN && num_args % 2 == 0)
            return PARSER_OK;

        pipe_ast->data.ast_pipe.right_arg = right_arg;
        num_args++;
    }

    return PARSER_OK;
}

static enum parser_status redirect_loop(struct ast **ast, struct lexer *lexer)
{
    while (1)
    {
        struct ast *redirect_node;
        enum parser_status status = parse_redirection(&redirect_node, lexer);
        if (status == PARSER_ERROR)
            return PARSER_ERROR;
        if (status == PARSER_UNKNOWN_TOKEN)
            return PARSER_OK;

        redirect_node->data.ast_redir.left = *ast;
        *ast = redirect_node;
    }

    return PARSER_OK;
}

enum parser_status parse_funcdec(struct ast **ast, struct lexer *lexer)
{
    struct lexer *lexer_cp = lexer_copy(lexer);

    struct token *token = lexer_peek(lexer_cp);
    if (token->type != TOKEN_WORD)
    {
        token_free(token);
        lexer_free(lexer_cp);
        return PARSER_UNKNOWN_TOKEN;
    }

    struct ast *ast_func = init_ast(AST_FUNCTION);
    struct ast_func *func = &ast_func->data.ast_func;

    func->name = calloc(token->len + 1, sizeof(char));
    func->name = strcpy(func->name, token->data);

    lexer_pop(lexer_cp);
    token_free(token);

    token = lexer_peek(lexer_cp);
    if (token->type != TOKEN_BRACKET_OPEN)
    {
        free_ast(ast_func);
        token_free(token);
        lexer_free(lexer_cp);
        return PARSER_UNKNOWN_TOKEN;
    }

    lexer_free(lexer_cp);
    lexer_pop(lexer); // word
    lexer_pop(lexer); // bracket

    token_free(token);
    *ast = ast_func;

    lexer_peek(lexer);
    if (token->type != TOKEN_BRACKET_CLOSE)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    lexer_pop(lexer);
    token_free(token);

    pop_duplicates(lexer, TOKEN_NEWLINE);

    struct ast *body;
    enum parser_status status = parse_shell_command(&body, lexer);
    if (status != PARSER_OK)
        return PARSER_ERROR;

    func->body = body;
    return PARSER_OK;
}

static enum parser_status func_aux(struct ast **ast, struct lexer *lexer)
{
    enum parser_status status = parse_funcdec(ast, lexer);
    if (status != PARSER_OK)
        return status;

    return redirect_loop(ast, lexer);
}

enum parser_status parse_command(struct ast **ast, struct lexer *lexer)
{
    // functions
    struct token *token = lexer_peek(lexer);
    enum parser_status status = func_aux(ast, lexer);

    if (status != PARSER_UNKNOWN_TOKEN)
    {
        token_free(token);
        return status;
    }

    // simple command
    if (token->type == TOKEN_WORD || token->type == TOKEN_ASSIGN)
    {
        enum parser_status status = parse_simple_command(ast, lexer);
        if (status != PARSER_UNKNOWN_TOKEN)
        {
            token_free(token);
            return status;
        }
    }

    token_free(token);

    // shell commands
    status = parse_shell_command(ast, lexer);

    if (status != PARSER_OK)
        return status;

    return redirect_loop(ast, lexer);
}

static enum redir_type parse_redir_type(char *str)
{
    if (!strcmp(str, ">>"))
        return STD_OUT_END;
    if (!strcmp(str, ">|"))
        return STD_RIGHT_ARROW_PIPE;
    if (!strcmp(str, ">"))
        return STD_OUT;
    if (!strcmp(str, "<"))
        return STD_IN;
    if (!strcmp(str, "<>"))
        return STD_IN_OUT;
    if (!strcmp(str, ">&"))
        return STD_ERR;
    return STD_LEFT_ARROW_AND;
}

enum parser_status parse_redirection(struct ast **ast, struct lexer *lexer)
{
    // CHECK IONUMBER 0 or 1 count
    int io = 0;
    int total = -1;

    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_WORD)
    {
        total = 0;
        for (int i = 0; i < token->len; i++)
        {
            if (!isdigit(token->data[i]))
            {
                token_free(token);
                return PARSER_UNKNOWN_TOKEN;
            }
            total *= 10;
            total += token->data[i] - '0';
        }
        io = 1;
        lexer_pop(lexer);
    }

    token_free(token);

    // CHECK IF TOKEN IS REDIRECT
    token = lexer_peek(lexer);

    if (token->type != TOKEN_REDIR)
    {
        token_free(token);
        if (io)
            return PARSER_ERROR;
        return PARSER_UNKNOWN_TOKEN;
    }

    struct ast *ast_redir = init_ast(AST_REDIR);
    *ast = ast_redir;

    (*ast)->data.ast_redir.ioNumber = total;
    (*ast)->data.ast_redir.type = parse_redir_type(token->data);

    token_free(token);
    lexer_pop(lexer);

    // CHECK IF LAST TOKEN IS WORD, WE DO NOT CARE WHATS AFTER
    token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
    {
        token_free(token);
        return PARSER_ERROR;
    }

    lexer_pop(lexer);
    struct ast *ast_file = init_ast(AST_FILE);
    ast_file->data.ast_file.filename = token->data;

    (*ast)->data.ast_redir.right = ast_file;

    free(token);
    return PARSER_OK;
}

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
    if (status == PARSER_UNKNOWN_TOKEN)
        return PARSER_OK;
    if (status != PARSER_OK)
        return status;

    // the ';'
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_COLON)
        lexer_pop(lexer);

    // the NEWLINES
    pop_duplicates(lexer, TOKEN_NEWLINE);

    token_free(token);
    return PARSER_OK;
}

enum parser_status parse_compound_list_rep(struct ast **ast,
                                           struct lexer *lexer)
{
    enum parser_status status = PARSER_OK;
    struct token *token;

    // create an ast list
    size_t index = 0;
    struct ast *ast_list = init_ast(AST_LIST);
    ast_list->data.ast_list.list[index++] = *ast;
    ast_list->data.ast_list.nb_nodes++;

    *ast = ast_list;

    while (status == PARSER_OK)
    {
        // the (';' | '\n')
        token = lexer_peek(lexer);
        if (token->type == TOKEN_COLON || token->type == TOKEN_NEWLINE)
            lexer_pop(lexer);

        // the {'\n'}
        pop_duplicates(lexer, TOKEN_NEWLINE);
        token_free(token);

        struct ast *node;
        status = parse_and_or(&node, lexer);
        if (status != PARSER_OK)
            return status;

        if (index == ast_list->data.ast_list.capacity)
            double_list_size(&ast_list->data.ast_list);

        ast_list->data.ast_list.list[index++] = node;
        ast_list->data.ast_list.nb_nodes++;

        token = lexer_peek(lexer);
        if (token->type == TOKEN_ASSIGN)
        {
            token_free(token);
            if (variable_list(ast, lexer, index) != PARSER_OK)
                return PARSER_ERROR;
        }
        else
            token_free(token);
    }

    return PARSER_OK;
}

void pop_duplicates(struct lexer *lexer, enum token_type type)
{
    struct token *token = lexer_peek(lexer);
    while (token->type == type)
    {
        lexer_pop(lexer);
        token_free(token);
        token = lexer_peek(lexer);
    }

    token_free(token);
}
