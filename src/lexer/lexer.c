#include "lexer.h"

#include <stdlib.h>

struct lexer *init_lexer(char *input)
{
    struct lexer *lex = malloc(sizeof(struct lexer));
    lex->input = input;
    lex->index = 0;
    lex->prev_t = NULL;
    return lex;
}

void lexer_free(struct lexer *lex)
{
    if (lex->prev_t)
        free(lex->prev_t);
    free(lex);
}

void lexer_next(struct lexer *lex)
{
    struct token *t = calloc(1, sizeof(struct token));

}
