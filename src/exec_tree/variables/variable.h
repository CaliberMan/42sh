#ifndef VARIABLE_H
#define VARIABLE_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../../exec/exec.h"

//variable struct
struct variable
{
    char *name;
    char *value;
};

//linked list of variables
struct variable_list
{
    struct variable *var;
    struct variable_list *next;
};

/**
 * @brief Frees the list of variables
 * @return Nothing
 */
void free_list_variables(void);

/**
 * @brief Adds or update the variable given
 * @param the name of the variable, it's type and it's data
 * @return 0 if it worked 1 if it failed
 */
int update_variable(char *name, char *new_value);

/**
 * @brief Find the variable by the name
 * @param the name of the variable
 * @return the variable pointer or NULL if it doesn't exist
 */

struct variable *find(char *name);

/**
 * @brief Unset the previously setted variable
 * @param the name of the variable you want to unset
 * @return 0 if it worked and 1 if it didn't find the variable
 */
int unset_variable(char *name);
int variable_expansion(struct exec_arguments command);
void init_variables(void);

#endif /* ! VARIABLE_H */
