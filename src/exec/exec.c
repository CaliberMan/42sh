#include "exec.h"

int exec(char **args)
{
    int f = fork();
    if (f < 0)
        return -1;

    // Child
    if (f == 0)
    {
        if (execvp(args[0], args) == -1)
            exit(127);
    }
    else
    {
        int status;
        waitpid(f, &status, 0);
        if (WIFEXITED(status))
        {
            int ex_st = WEXITSTATUS(status);
            if (ex_st == 127)
                return -1;
            else if (ex_st == 1)
                return 1;
        }
    }

    return 0;
}