#include "var_utils.h"
#include "../../exec/exec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

void free_single_var(struct variable_list *actual)
{
    free(actual->var->name);
    free(actual->var->value);
    free(actual->var);
    free(actual);
}

struct function_list *make_function(char *name, struct ast *ast)
{
    size_t name_len = strlen(name) + 1;
    struct function_list *list = calloc(1, sizeof(struct function_list));
    if (!list)
        return NULL;
    list->next = NULL;

    struct function *var = calloc(1, sizeof(struct function));
    if (!var)
        return NULL;

    var->name = calloc(1, name_len + 1);
    memcpy(var->name, name, name_len);
    var->body = ast;

    return list;
}

struct variable_list *make_variable(char *name, char *new_value)
{
    size_t name_len = strlen(name) + 1;
    struct variable_list *list = calloc(1, sizeof(struct variable_list));
    if (!list)
        return NULL;
    list->next = NULL;

    struct variable *var = calloc(1, sizeof(struct variable));
    if (!var)
        return NULL;

    size_t len_var = strlen(new_value);
    var->value = calloc(1, len_var + 1);
    memcpy(var->value, new_value, len_var);
    var->name = calloc(1, name_len + 1);
    memcpy(var->name, name, name_len);
    list->var = var;

    return list;
}

size_t get_longest_valid_name(char *str, size_t i)
{
    size_t j = 0;
    while (str[j + i] && isalnum(str[j + i]))
        j++;
    return j;
}

char *replace_str(char *ptr, char *str, size_t before_declaration,
                         char *after_word)
{
    if (*after_word == '}')
        after_word++;
    size_t ptr_len = strlen(ptr);
    size_t str_len = strlen(str);
    size_t after_len = strlen(after_word);
    char *new_guy = calloc(1, ptr_len + str_len);
    memcpy(new_guy, str, before_declaration);
    memcpy(new_guy + before_declaration, ptr, ptr_len);
    memcpy(new_guy + before_declaration + ptr_len, after_word, after_len);
    return new_guy;
}
