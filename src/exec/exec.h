#ifndef EXEC_H
#define EXEC_H

#include <stdio.h>
#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

/**
 * @brief Forks the process and runs execvp
 * @param arguments for the command
 * @return Returns -1 if the execution failled, 1 if the output of the excecution is false, 0 if everything went well
 */

int exec(char **args);

#endif /* ! EXEC_H */
