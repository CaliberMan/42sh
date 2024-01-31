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
 * @param number returned
 * @return Returns number given
 */
int b_exit(struct exec_arguments command);

#endif /* ! BUILTINS_H */
