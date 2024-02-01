#ifndef VAR_UTILS_H
#define VAR_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../ast/ast.h"

// function struct
struct function
{
    char *name;
    struct ast *body;
};

// function linked list
struct function_list
{
    struct function_list *next;
    struct function *func;
};

//variable struct
struct variable
{
    char *name;
    char *value;
};


// linked list of variables
struct variable_list
{
    struct variable_list *next;
    struct variable *var;
};

struct global_list
{
    struct variable_list *var_list;
    struct function_list *func_list;
};

void free_single_var(struct variable_list *actual);
struct variable_list *make_variable(char *name, char *new_value);
size_t get_longest_valid_name(char *str, size_t i);
char *replace_str(char *ptr, char *str, size_t before_declaration,
                  char *after_word);
struct function_list *make_function(char *name, struct ast *ast);

#endif /* ! VAR_UTILS_H */
