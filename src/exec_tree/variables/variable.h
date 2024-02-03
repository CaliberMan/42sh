#ifndef VARIABLE_H
#define VARIABLE_H

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../../exec/exec.h"
#include "../var_utils/var_utils.h"

/**
 * @brief Frees the list of variables
 * @return Nothing
 */
void free_list_global(void);

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
struct variable *find_var(char *name);
/**
 * @brief Find the function by the name
 * @param the name of the function
 * @return the function pointer or NULL if it doesn't exist
 */
struct function *find_func(char *name);

/**
 * @brief Unset the previously setted variable
 * @param the name of the variable you want to unset
 * @return 0 if it worked and 1 if it didn't find the variable
 */
int unset_variable(char *name);
int variable_expansion(struct exec_arguments command);
void init_variables(char *arg_list[]);

int update_function(char *name, struct ast *ast);
int unset_function(char *name);

void set_global_list(struct global_list *new_list);
struct global_list *get_global_list(void);

#endif /* ! VARIABLE_H */
