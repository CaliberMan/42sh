#pragma once

enum token_type
{
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_FI,
    TOKEN_COLON,
    TOKEN_NEWLINE,
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIR,
    TOKEN_NOT,
    TOKEN_WHILE,
    TOKEN_UNTIL,
    TOKEN_DO,
    TOKEN_DONE,
    TOKEN_FOR,
    TOKEN_IN,
    TOKEN_EOF,
    TOKEN_START,
    TOKEN_ERROR
};

struct token
{
    char *data;
    int len;
    int capacity;
    enum token_type type;
};

struct lexer
{
    int cmd_status;
    char *input;
    int index;
    struct token *prev_token;
};

/**
 * @brief sets up a lexer with a given input string
 * @param input string that the lexer will lex
 * @return lexer
 */

struct lexer *init_lexer(char *input);

/**
 * @brief free the given token
 * @param token to be freed
 * @return void * to NULL
 */

void *token_free(struct token *t);

/**
 * @brief free the given lexer
 * @param lexer to be freed
 * @return nothing
 */

void lexer_free(struct lexer *lex);

/**
 * @brief retreives the next token in the lexer, updates the index, and puts the
 * token lexed in prev_token
 * @param lexer
 * @return token lexed
 */

struct token *lexer_pop(struct lexer *lex);

/**
 * @brief same behaviour as lexer_pop but it does not update either the index or
 * the prev_token
 * @param lexer
 * @return token lexed
 */

struct token *lexer_peek(struct lexer *lex);

// #endif /* ! LEXER_H */
