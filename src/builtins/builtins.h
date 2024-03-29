#ifndef BUILTINS_H
#define BUILTINS_H

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../exec/exec.h"
#include "../exec_tree/variables/variable.h"

/**
 * @brief builtin returning 0, representing truth
 * @return 0
 */
int b_true(void);
/**
 * @brief builtin returning 1, representing falsehood
 * @return 1
 */
int b_false(void);
/**
 * @brief simulate echo builtin with options -n, -e, -E
 * @param arguments for echo
 * @return Returns -1 if the execution faied,
 * 1 if the output of the excecution is false, 0 if everything went well
 */
int b_echo(struct exec_arguments command);
/**
 * @brief simulate unset
 * @param arguments for unset
 * @return Returns 0 even if it failled
 */
int b_unset(struct exec_arguments command);

/**
 * @brief simulate cd with option of '-'
 * @param arguments for cd
 * @return Returns 1 if the execution failed.
 */
int b_cd(struct exec_arguments command);
/**
 * @brief simulate exit
 * @param arguments exit
 * @return Returns whatever the user wants or 0, and -1 if to many args
 */
int b_exit(struct exec_arguments command);
/**
 * @brief simulate dot with input variables
 * @param the file followed by all the input arguments of the file
 * @return Returns whatever the file is outputins
 */
int b_dot(struct exec_arguments command);
/**
 * @brief simulate the break
 * @param the number of loops it wants to escape
 * @return Returns the number of loops (0 if not specified) -1 if wrong args
 */
int b_break(struct exec_arguments command);
/**
 * @brief simulate the break
 * @param the number of loops it wants to escape
 * @return Returns the number of loops (0 if not specified) -1 if wrong args
 */
int b_continue(struct exec_arguments command);
#endif /* ! BUILTINS_H */
