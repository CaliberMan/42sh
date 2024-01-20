#ifndef EXEC_H
#define EXEC_H

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @brief struct to pass the arguments of functions
 * @param  pipe file descriptors, a child process id, and the arguments for
 * command
 * @return Returns -1 if the execution failled, 1 if the output of the
 * excecution is false, 0 if everything went well
 */
struct exec_arguments
{
    int pipe_fds[2];
    int child_process;
    char **args;
};

/**
 * @brief Forks the process and runs execvp
 * @param arguments for the command
 * @return Returns -1 if the execution failled, 1 if the output of the
 * excecution is false, 0 if everything went well
 */

int exec(struct exec_arguments command);

#endif /* ! EXEC_H */
