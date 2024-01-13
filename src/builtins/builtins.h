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
 * @return Returns -1 if the execution failled,
 * 1 if the output of the excecution is false, 0 if everything went well
 */
int b_echo(char **args);

#endif /* ! BUILTINS_H */
