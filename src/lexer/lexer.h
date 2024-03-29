#ifndef LEXER_H
#define LEXER_H

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
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_IONUM,
    TOKEN_ASSIGN,
    TOKEN_CURLY_OPEN,
    TOKEN_CURLY_CLOSE,
    TOKEN_BRACKET_OPEN,
    TOKEN_BRACKET_CLOSE,
    TOKEN_ASSIGNMENT_WORD,
    TOKEN_CASE,
    TOKEN_ESAC,
    TOKEN_DOUBLE_COLON,
    TOKEN_ERROR
};

enum lexer_status
{
    LEXER_OK,
    LEXER_IN,
    LEXER_DO
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
    char *input;
    int index;
    struct token *prev_token;
    enum lexer_status status;
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

/**
 * @brief same behaviour as lexer_pop but it does not update either the index or
 * the prev_token
 * @param lexer
 * @return token lexed
 */

struct lexer *lexer_copy(struct lexer *lex);

#endif /* ! LEXER_H */
