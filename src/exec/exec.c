#include "exec.h"

#include <stdio.h>

int exec(struct exec_arguments command)
{
    int f = fork();
    if (f < 0)
        return -1;

    // Child
    if (f == 0)
    {
        if (execvp(command.args[0], command.args) == -1)
        {
            fprintf(stderr, "%s: command not found\n", command.args[0]);
            exit(127);
        }
    }
    else
    {
        int status;
        waitpid(f, &status, 0);
        if (WIFEXITED(status))
        {
            int ex_st = WEXITSTATUS(status);
            return ex_st;
        }
    }

    return 0;
}
