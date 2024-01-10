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
    TOKEN_STRING,
    TOKEN_EOF,
    TOKEN_START,
    TOKEN_ERROR
};

struct token
{
    char *data;
    enum token_type type;
};

struct lexer
{
    char *input;
    int index;
    struct token *prev_t;
};

/**
 * @brief
 * @param
 * @return
 */



#endif /* ! LEXER_H */
