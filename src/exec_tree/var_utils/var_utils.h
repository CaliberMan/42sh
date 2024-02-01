#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//variable struct
struct variable
{
    char *name;
    char *value;
};

// linked list of variables
struct variable_list
{
    struct variable *var;
    struct variable_list *next;
};

void free_single_var(struct variable_list *actual);
struct variable_list *make_variable(char *name, char *new_value);
size_t get_longest_valid_name(char *str, size_t i);
char *replace_str(char *ptr, char *str, size_t before_declaration,
                  char *after_word);
