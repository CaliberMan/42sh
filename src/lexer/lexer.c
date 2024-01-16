#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lexer *init_lexer(char *input)
{
    struct lexer *lex = calloc(1, sizeof(struct lexer));
    if (!lex)
        return NULL;
    struct token *t = calloc(1, sizeof(struct token));
    if (!t)
    {
        free(lex);
        return NULL;
    }
    t->data = NULL;
    t->type = TOKEN_START;
    lex->prev_token = t;
    lex->input = input;
   return lex;
}

void *token_free(struct token *t)
{
    if (t)
    {
        if (t->data)
            free(t->data);
        free(t);
    }
    return NULL;
}

void lexer_free(struct lexer *lex)
{
    if (lex->prev_token)
        token_free(lex->prev_token);
    free(lex);
}

int increase_capacity(struct token *t)
{
    t->data = realloc(t->data, sizeof(char) * t->capacity * 2);
    if (!t->data)
        return 1;
    t->capacity *= 2;
    for (int i = t->len; i < t->capacity; i++)
        t->data[i] = 0;
    return 0;
}

int valid_char(char c)
{
    return c != ' ' && c != ';' && c != '\n' && c != 0;
}

int init_token(struct lexer *lex, struct token *t)
{
    if (lex->prev_token->type != TOKEN_WORD)
    {
        if (!strcmp("if", t->data))
            t->type = TOKEN_IF;
        else if (!strcmp("else", t->data))
            t->type = TOKEN_ELSE;
        else if (!strcmp("then", t->data))
            t->type = TOKEN_THEN;
        else if (!strcmp("fi", t->data))
            t->type = TOKEN_FI;
        else
            t->type = TOKEN_WORD;
    }
    else
        t->type = TOKEN_WORD;
    return 0;
}

enum token_type single_char_tokens(struct lexer *lex)
{
    enum token_type tt = TOKEN_ERROR;
    if (lex->input[lex->index] == ';')
        tt = TOKEN_COLON;
    if (lex->input[lex->index] == '\n')
        tt = TOKEN_NEWLINE;
    if (lex->input[lex->index] == 0)
        tt = TOKEN_EOF;
    lex->index++;
    return tt;
}

struct token *lexer_pop(struct lexer *lex)
{
    struct token *t = calloc(1, sizeof(struct token));
    if (!t)
        return NULL;
    t->data = calloc(10, sizeof(char));
    if (!t->data)
        return token_free(t);
    t->capacity = 10;
    while (lex->input[lex->index] == ' ')
        lex->index++;
    while (valid_char(lex->input[lex->index]))
    {
        if (lex->input[lex->index] == '\'')
        {
            lex->index++;
            while (lex->input[lex->index] != '\''
                   && lex->input[lex->index] != 0)
            {
                t->data[t->len] = lex->input[lex->index];
                t->len++;
                if (t->len == t->capacity)
                {
                    int res = increase_capacity(t);
                    if (res)
                        token_free(t);
                }
                lex->index++;
            }
            if (lex->input[lex->index] == 0)
                return token_free(t);
        }
        else
        {
            t->data[t->len] = lex->input[lex->index];
            t->len++;
            if (t->len == t->capacity)
            {
                int res = increase_capacity(t);
                if (res)
                    token_free(t);
            }
        }
        lex->index++;
    }
    if (t->data[0] == 0)
        t->type = single_char_tokens(lex);
    else
    {
        int res = init_token(lex, t);
        if (res)
            return NULL;
    }
    token_free(lex->prev_token);
    lex->prev_token = t;
    return t;
}

enum token_type single_char_tokens_peek(struct lexer *lex, int index)
{
    enum token_type tt = TOKEN_ERROR;
    if (lex->input[index] == ';')
        tt = TOKEN_COLON;
    if (lex->input[index] == '\n')
        tt = TOKEN_NEWLINE;
    if (lex->input[index] == 0)
        tt = TOKEN_EOF;
    return tt;
}

struct token *lexer_peek(struct lexer *lex)
{
    struct token *t = calloc(1, sizeof(struct token));
    if (!t)
        return NULL;
    t->data = calloc(10, sizeof(char));
    if (!t->data)
        return token_free(t);
    int index = lex->index;
    t->capacity = 10;
    while (lex->input[index] == ' ')
        index++;
    while (valid_char(lex->input[index]))
    {
        if (lex->input[index] == '\'')
        {
            lex->index++;
            while (lex->input[index] != '\'' && lex->input[index] != 0)
            {
                t->data[t->len] = lex->input[index];
                t->len++;
                if (t->len == t->capacity)
                {
                    int res = increase_capacity(t);
                    if (res)
                        token_free(t);
                }
                lex->index++;
            }
            if (lex->input[index] == 0)
                return token_free(t);
        }
        else
        {
            t->data[t->len] = lex->input[index];
            t->len++;
            if (t->len == t->capacity)
            {
                int res = increase_capacity(t);
                if (res)
                    return token_free(t);
            }
        }
        index++;
    }
    if (t->data[0] == 0)
    {
        t->type = single_char_tokens_peek(lex, index);
        index++;
    }
    else
    {
        int res = init_token(lex, t);
        if (res)
            return NULL;
    }
    return t;
}
