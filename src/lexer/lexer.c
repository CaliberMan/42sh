#include "lexer.h"

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
    lex->input = input;
    return lex;
}

void lexer_free(struct lexer *lex)
{
    if (lex->prev_t)
        free(lex->prev_t);
    free(lex);
}

int increase_capacity(struct token* t)
{
	t->data = realloc(t->data, sizeof(char) * t->capacity * 2);
	if (!t->data)
		return 1;
	t->capacity *= 2;
	for (int i = t->len; i < t->capacity; i++)
		t->data[i] == 0;
	return 1;
}

int valid_char(char c)
{
	return c != ' ' &&
	       c != ';' &&
	       c != '\n' &&
	       c != 0;
}

int init_token(struct lex *lex, struct token *t)
{
	if (lex->prev_token->type != TOKEN_WORD)
	{
		if (!strcmp("if", t->data))
			t->type = TOKEN_IF;
		elif (!strcmp("else", t->data))
			t->type = TOKEN_ELSE;
		elif (!strcmp("then", t->data))
			t->type = TOKEN_THEN;
		elif (!strcmp("fi", t->data))
			t->type = TOKEN_FI;
		else
			t->type = TOKEN_WORD;
	}
	else
		t->type = TOKEN_WORD;
	return 0;
}

struct token *lexer_pop(struct lexer *lex)
{
    struct token *t = calloc(1, sizeof(struct token));
    if (!t)
	    return NULL;
    t->data = calloc(10, sizeof(char));
    if (!t->data)
    {
	    free(t);
	    return 1;
    }
    t->capacity = 10;
    while (lex->input[lex->index] == ' ')
	    lex->input++;
    while (valid_char(lex->input[lex->index]))
    {
	    //       			   |
	    //				   |
	    //CHKQWKLIE DSA ]'             | ?????????????????????
	    //				  \ /
	    //				   v
	    if (lex->input[lex->index] == ''')
	    {
		    while (lex->input[lex->index] != ''' &&
		           lex->input[lex->index] != 0)
		    {
			    t->data[t->len] = lex->input[lex->index];
	   	 	    t->len++;
			    if (t->len == t->capacity)
			    {
				    int res = increase_capacity(t);
				    if (res)
					    free(t);
			    }
			    lex->index++;
		    }
		    if (lex->input[lex->index] == 0)
		    {
			    free(t->data);
			    free(t);
			    return NULL;
		    }
	    }
	    else
	    {
		    t->data[t->len] = lex->input[lex->index];
		    t->len++;
		    if (t->len == t->capacity)
		    {
			    int res = increase_capacity(t);
			    if (res)
				    free(t);
		    }
	    }
	    lex->input++;
    }
    if (t->data[0] == 0)
    {
	    if (lexer->input[lexer->index] == ';')
		    t->type = TOKEN_COLON;
	    if (lexer->input[lexer->index] == '\n')
		    t->type = TOKEN_NEWLINE;
	    if (lexer->input[lexer->index] == 0)
		    t->type = TOKEN_EOF;
	    lexer->index++;
    }
    else
    	int res = init_token(lex, t);
    if (!res)
	    return NULL;
    free(lex->prev_token);
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
    {
	    free(t);
	    return 1;
    }
    int index = lex->index;
    t->capacity = 10;
    while (lex->input[index] == ' ')
	    input++;
    while (valid_char(input[lex->index]))
    {
	    if (lex->input[lex->index] == ''')
	    {
		    while (lex->input[lex->index] != ''' &&
		           lex->input[lex->index] != 0)
		    {
			    t->data[t->len] = lex->input[lex->index];
	   	 	    t->len++;
			    if (t->len == t->capacity)
			    {
				    int res = increase_capacity(t);
				    if (res)
					    free(t);
			    }
			    lex->index++;
		    }
		    if (lex->input[lex->index] == 0)
		    {
			    free(t->data);
			    free(t);
			    return NULL;
		    }
	    }
	    else
	    {
		    t->data[t->len] = lex->input[lex->index];
		    t->len++;
		    if (t->len == t->capacity)
		    {
			    int res = increase_capacity(t);
			    if (res)
				    free(t);
		    }
	    }
	    input++;
    }
    if (t->data[0] == 0)
    {
	    if (lexer->input[lexer->index] == ';')
		    t->type = TOKEN_COLON;
	    if (lexer->input[lexer->index] == '\n')
		    t->type = TOKEN_NEWLINE;
	    if (lexer->input[lexer->index] == 0)
		    t->type = TOKEN_EOF;
    }
    else
    	int res = init_token(lex, t);
    if (!res)
	    return NULL;
    return t;
}

int main(void)
{
	struct lexer *lex = init_lexer("echo")
}
