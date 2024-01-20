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
    if (lex->input)
	free(lex->input);
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

int valid_char(struct lexer *lex, int *index)
{
    char c = lex->input[*index];
    return c != ' ' &&
	   c != ';' &&
	   c != '\n' &&
	   c != 0 &&
	   c != '<' &&
	   c != '>' &&
	   !((c == '&' || c == '|') && lex->input[(*index) + 1] == c);
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
	else if (!strcmp("elif", t->data))
            t->type = TOKEN_ELIF;
	else if (!strcmp("|", t->data))
	    t->type = TOKEN_PIPE;
	else if (!strcmp("!", t->data))
	    t->type = TOKEN_NOT;
        else if (!strcmp("while", t->data))
	    t->type = TOKEN_WHILE;
	else if (!strcmp("until", t->data))
	    t->type = TOKEN_UNTIL;
 	else if (!strcmp("do", t->data))
	    t->type = TOKEN_DO;
 	else if (!strcmp("done", t->data))
	    t->type = TOKEN_DONE;
  	else
            t->type = TOKEN_WORD;
    }
    else
        t->type = TOKEN_WORD;
    return 0;
}

enum token_type single_char_tokens(struct lexer *lex, struct token *t, int index)
{
    t->data[0] = lex->input[index];
    enum token_type tt = TOKEN_ERROR;
    if (lex->input[index] == ';')
        tt = TOKEN_COLON;
    else if (lex->input[index] == '\n')
        tt = TOKEN_NEWLINE;
    else if (lex->input[index] == 0)
	    tt = TOKEN_EOF;
    else if (lex->input[index] == '<' ||
  	     lex->input[index] == '>')
    {
        if (lex->input[index + 1] == '>' ||
	    lex->input[index + 1] == '&' ||
	    (lex->input[index] == '>' &&
	     lex->input[index + 1] == '|'))
	    t->data[1] = lex->input[index + 1];
	tt = TOKEN_REDIR;
    }
    else if ((lex->input[index] == '&' ||
	      lex->input[index] == '|') &&
	     lex->input[index + 1] == lex->input[index]) 
        t->data[1] = lex->input[index];
    t->len = strlen(t->data);
    return tt;
}

int pop_traverse(struct lexer *lex, struct token *t, int *index)
{
    while (lex->input[*index] == ' ')
        (*index)++;
    if (lex->input[*index] == '#')
    {
	    while (lex->input[*index] != '\n' && lex->input[*index])
		    (*index)++;
    }
    while (valid_char(lex, index))
    {
	
        if (lex->input[*index] == '\'')
        {
            (*index)++;
            while (lex->input[*index] != '\''
                   && lex->input[*index] != 0)
            {
                t->data[t->len] = lex->input[*index];
                t->len++;
                if (t->len == t->capacity)
                {
                    int res = increase_capacity(t);
                    if (res)
                        return 1;
                }
                (*index)++;
            }
            if (lex->input[*index] == 0)
                return 1;
        }
        else
        {
            t->data[t->len] = lex->input[*index];
            t->len++;
            if (t->len == t->capacity)
            {
                int res = increase_capacity(t);
                if (res)
                    return 1;
            }
        }
        (*index)++;
    }
    return 0;
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
    int res = pop_traverse(lex, t, &(lex->index));
    if (res)
	return token_free(t);
    if (t->data[0] == 0)
    {
        t->type = single_char_tokens(lex, t, lex->index);
    	lex->index += t->len;
    }
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

struct token *lexer_peek(struct lexer *lex)
{
    struct token *t = calloc(1, sizeof(struct token));
    if (!t)
        return NULL;
    t->data = calloc(10, sizeof(char));
    if (!t->data)
        return token_free(t);
    t->capacity = 10;
    int index = lex->index;
    int res = pop_traverse(lex, t, &index);
    if (res)
	return token_free(t);
    if (t->data[0] == 0)
        t->type = single_char_tokens(lex, t, index);
    else
    {
        int res = init_token(lex, t);
        if (res)
            return NULL;
    }
    return t;
}
/*
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
            index++;
            while (lex->input[index] != '\'' && lex->input[index] != 0)
            {
                t->data[t->len] = lex->input[index];
                t->len++;
                if (t->len == t->capacity)
                {
                    int res = increase_capacity(t);
                    if (res)
                        return token_free(t);
                }
                index++;
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
        t->type = single_char_tokens(lex, index);
        index += t->len;
    }
    else
    {
        int res = init_token(lex, t);
        if (res)
            return NULL;
    }
    return t;
}*/

int main(void)
{
    struct lexer *lex = init_lexer("if ! echo a; then echo b; fi; >> >& >| <> <& > < | test>test ! #finish\n#finish2");
    printf("full string: %s\n", lex->input);
    struct token *t = lexer_peek(lex);
    token_free(t);
    while (t->type != TOKEN_EOF)
    {
        t = lexer_peek(lex);
	printf("peek: %s, len: %d\n", t->data, t->len);
	token_free(t);
	t = lexer_pop(lex);
    }
}
