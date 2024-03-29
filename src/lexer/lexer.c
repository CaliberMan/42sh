#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../exec_tree/var_utils/var_utils.h"
#include "../exec_tree/variables/variable.h"
#include "../utils/utils_main.h"

FILE *f = NULL;

static void run_main(char **buffer, char *str)
{
    struct global_list *temp = get_global_list();

    int is_main = 0;
    if (isatty(1))
        is_main = 1;
    if (is_main)
        f = freopen("subshell_output_file.txt", "w+", stdout);
    int argc = 3;
    char **argv = calloc(4, sizeof(char *));
    char *name = "alt42";
    char *c = "-c";
    argv[0] = name;
    argv[1] = c;
    argv[2] = str;
    main(argc, argv);
    free(argv);
    set_global_list(temp);

    long length;
    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        *buffer = calloc(length + 1, sizeof(char));
        if (*buffer)
            fread(*buffer, 1, length, f);
    }
    if (is_main)
    {
        f = freopen("subshell_output_file.txt", "w+", f);
        fclose(f);
        freopen ("/dev/tty", "a+", stdout);
        remove("subshell_output_file.txt");
    }
    else
        f = freopen("subshell_output_file.txt", "w+", f);
}

static int find_close(char *str, int index)
{
    int depth = 1;
    while (str[index] && depth)
    {
        if (str[index] == '"')
            while (str[index] && !(str[index] == '"' && str[index - 1] == '\\'))
                index++;
        else if (str[index] == '\'')
            while (str[index] && str[index] != '\'')
                index++;
        else if (str[index] == '(')
            depth++;
        else if (str[index] == ')')
            depth--;
        index++;
    }
    if (depth != 0)
        return -1;
    return index;
}

static int expand_subshells(struct lexer *lexer)
{
    for (int i = 0; lexer->input[i]; i++)
    {
        if (lexer->input[i] == '$' && lexer->input[i + 1] == '(')
        {
            int end = find_close(lexer->input, i + 2);
            if (end == -1)
                return 1;
            char *str = calloc(end - i, sizeof(char));
            for (int j = i + 2; j < end - 1; j++)
                str[j - i - 2] = lexer->input[j];
            char *buffer = NULL;
            run_main(&buffer, str);
            char *new_input =
                calloc(strlen(lexer->input) + strlen(buffer), sizeof(char));
            free(str);
            int index = 0;
            while (index < i)
            {
                new_input[index] = lexer->input[index];
                index++;
            }
            int index_b = 0;
            while (buffer[index_b])
            {
                if (buffer[index_b] == '\n')
                    new_input[index] = ' ';
                else
                    new_input[index] = buffer[index_b];
                index++;
                index_b++;
            }
            while (lexer->input[end])
            {
                new_input[index] = lexer->input[end];
                index++;
                end++;
            }
            if (buffer)
                free(buffer);
            free(lexer->input);
            lexer->input = new_input;
        }
    }
    return 0;
}

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
    lex->status = LEXER_OK;
    expand_subshells(lex);
    return lex;
}

static int better_len(char *str)
{
    int count = 0;
    for (int i = 0; str[i]; i++)
        count++;
    return count;
}

struct token *token_copy(struct token *t)
{
    struct token *new = calloc(1, sizeof(struct token));
    if (!new)
        return NULL;
    if (t->data)
    {
        int len = better_len(t->data);
        char *str = calloc(len + 1, sizeof(char));
        if (!str)
        {
            token_free(new);
            return NULL;
        }
        for (int i = 0; t->data[i]; i++)
            str[i] = t->data[i];
        new->data = str;
        new->len = len;
    }
    else
        new->data = NULL;
    new->capacity = t->capacity;
    new->type = t->type;
    return new;
}

struct lexer *lexer_copy(struct lexer *lex)
{
    char *str = calloc(better_len(lex->input) + 1, sizeof(char));
    if (!str)
        return NULL;
    for (int i = 0; lex->input[i]; i++)
        str[i] = lex->input[i];
    struct lexer *new = calloc(1, sizeof(struct lexer));
    if (!lex)
    {
        free(str);
        return NULL;
    }
    new->index = lex->index;
    new->input = str;
    new->prev_token = token_copy(lex->prev_token);
    new->status = lex->status;
    return new;
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

static int increase_capacity(struct token *t)
{
    t->data = realloc(t->data, sizeof(char) * t->capacity * 2);
    if (!t->data)
        return 1;
    t->capacity *= 2;
    for (int i = t->len; i < t->capacity; i++)
        t->data[i] = 0;
    return 0;
}

static int valid_char(struct lexer *lex, int *index)
{
    char c = lex->input[*index];
    return c != ' ' && c != ';' && c != '\n' && c != 0 && c != '<' && c != '>'
        && c != '|' && c != '(' && c != ')'
        && !((c == '&' || c == '|') && lex->input[(*index) + 1] == c);
}

static void init_token_2(struct lexer *lex, struct token *t, int pop)
{
    if (!strcmp("if", t->data))
        t->type = TOKEN_IF;
    else if (!strcmp("else", t->data))
        t->type = TOKEN_ELSE;
    else if (!strcmp("then", t->data))
        t->type = TOKEN_THEN;
    else if (!strcmp("fi", t->data))
        t->type = TOKEN_FI;
    else if (!strcmp("in", t->data))
    {
        if (pop)
            lex->status = LEXER_OK;
        t->type = TOKEN_IN;
    }
    else if (!strcmp("elif", t->data))
        t->type = TOKEN_ELIF;
    else if (!strcmp("!", t->data))
        t->type = TOKEN_NOT;
    else if (!strcmp("{", t->data))
        t->type = TOKEN_CURLY_OPEN;
    else if (!strcmp("}", t->data))
        t->type = TOKEN_CURLY_CLOSE;
    else if (!strcmp("while", t->data))
        t->type = TOKEN_WHILE;
    else if (!strcmp("until", t->data))
        t->type = TOKEN_UNTIL;
    else if (!strcmp("do", t->data))
        t->type = TOKEN_DO;
    else if (!strcmp("done", t->data))
        t->type = TOKEN_DONE;
    else if (!strcmp("for", t->data))
    {
        if (pop)
            lex->status = LEXER_IN;
        t->type = TOKEN_FOR;
    }
    else if (!strcmp("case", t->data))
    {
        if (pop)
            lex->status = LEXER_IN;
        t->type = TOKEN_CASE;
    }
    else if (!strcmp("esac", t->data))
        t->type = TOKEN_ESAC;
    else
        t->type = TOKEN_WORD;
}

static int init_token(struct lexer *lex, struct token *t, int pop)
{
    if (lex->prev_token->type != TOKEN_WORD)
        init_token_2(lex, t, pop);
    else if (lex->status == LEXER_IN)
    {
        if (!strcmp("in", t->data))
        {
            if (pop)
                lex->status = LEXER_OK;
            t->type = TOKEN_IN;
        }
        else
            t->type = TOKEN_WORD;
    }
    else
        t->type = TOKEN_WORD;
    return 0;
}

static enum token_type single_char_tokens(struct lexer *lex, struct token *t,
                                          int index)
{
    t->data[0] = lex->input[index];
    enum token_type tt = TOKEN_ERROR;
    if (lex->input[index] == ';')
    {
        if (lex->input[index + 1] == ';')
        {
            t->data[1] = ';';
            tt = TOKEN_DOUBLE_COLON;
        }
        else
            tt = TOKEN_COLON;
    }
    else if (lex->input[index] == '\n')
        tt = TOKEN_NEWLINE;
    else if (lex->input[index] == '&' && lex->input[index + 1] == '&')
    {
        t->data[1] = lex->input[index];
        tt = TOKEN_AND;
    }
    else if (lex->input[index] == '|' && lex->input[index + 1] == '|')
    {
        t->data[1] = lex->input[index];
        tt = TOKEN_OR;
    }
    else if (lex->input[index] == 0)
        tt = TOKEN_EOF;
    else if (lex->input[index] == '|')
        tt = TOKEN_PIPE;
    else if (lex->input[index] == '(')
        tt = TOKEN_BRACKET_OPEN;
    else if (lex->input[index] == ')')
        tt = TOKEN_BRACKET_CLOSE;

    else if (lex->input[index] == '<' || lex->input[index] == '>')
    {
        if (lex->input[index + 1] == '>' || lex->input[index + 1] == '&'
            || (lex->input[index] == '>' && lex->input[index + 1] == '|'))
            t->data[1] = lex->input[index + 1];
        tt = TOKEN_REDIR;
    }
    t->len = strlen(t->data);
    return tt;
}

static int lex_string(struct lexer *lex, struct token *t, int *index)
{
    if (lex->input[*index] == '\'')
    {
        (*index)++;
        while (lex->input[*index] != '\'' && lex->input[*index] != 0)
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
        (*index)++;
        while (lex->input[*index] != '"' && lex->input[*index] != 0)
        {
            if (lex->input[*index] == '\\'
                && (lex->input[*index + 1] == '$'
                    || lex->input[*index + 1] == '\''
                    || lex->input[*index + 1] == '"'
                    || lex->input[*index + 1] == '\\'
                    || lex->input[*index + 1] == '\n'))
            {
                (*index)++;
                if (lex->input[*index] == 0)
                    return 1;
            }
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
    return 0;
}

static int pop_traverse(struct lexer *lex, struct token *t, int *index)
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
        if (lex->input[*index] == '=' && t->len > 0
            && lex->prev_token->type != TOKEN_WORD)
        {
            int valid = 1;
            if (!(isalpha(t->data[0]) || t->data[0] == '_'))
                valid = 0;
            else
            {
                for (int i = 1; t->data[i]; i++)
                    if (!(isalnum(t->data[i]) || t->data[i] == '_'))
                        valid = 0;
            }
            if (valid)
            {
                (*index)++;
                t->type = TOKEN_ASSIGNMENT_WORD;
                return 0;
            }
        }
        if (lex->input[*index] == '\'' || lex->input[*index] == '"')
        {
            int res = lex_string(lex, t, index);
            if (res)
                return res;
        }
        else if (!(lex->input[*index] == '$' && lex->input[*index + 1] == '('))
        {
            if (lex->input[*index] == '\\')
            {
                if (!lex->input[*index + 1])
                    return 1;
                (*index)++;
                if (!lex->input[*index])
                    return 1;
            }
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
    else if (t->type != TOKEN_ASSIGNMENT_WORD)
    {
        if (lex->input[lex->index] == '>' || lex->input[lex->index] == '<')
        {
            int io = 1;
            for (int i = 0; i < t->len; i++)
                if (!isdigit(t->data[0]))
                    io = 0;
            if (io)
            {
                t->type = TOKEN_IONUM;
                token_free(lex->prev_token);
                lex->prev_token = t;
                return t;
            }
        }
        int res = init_token(lex, t, 1);
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
    else if (t->type != TOKEN_ASSIGNMENT_WORD)
    {
        if (lex->input[index] == '>' || lex->input[index] == '<')
        {
            int io = 1;
            for (int i = 0; i < t->len; i++)
                if (t->data[i] > '9' && t->data[i] < '0')
                    io = 0;
            if (io)
            {
                t->type = TOKEN_IONUM;
                return t;
            }
        }
        int res = init_token(lex, t, 0);
        if (res)
            return NULL;
    }
    return t;
}
