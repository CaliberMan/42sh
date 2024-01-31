#include "builtins.h"

int b_true(void)
{
    return 0;
}

int b_false(void)
{
    return 1;
}

int b_echo(struct exec_arguments command)
{
    char **args = command.args;
    size_t i = 1;
    int newline = 1;
    int interpreted = 0;
    int out = 0;
    for (; args[i]; i++)
    {
        if (args[i][0] != '-')
            break;
        for (size_t j = 1; args[i][j]; j++)
        {
            if (args[i][j] == 'n')
                newline = 0;
            // use puts
            else if (args[i][j] == 'E')
                interpreted = 0;
            else if (args[i][j] == 'e')
                interpreted = 1;
            else
            {
                // just a string starting with '-'
                out = 1;
                break;
            }
        }
        if (out)
            break;
    }
    if (interpreted)
    {
        for (; args[i]; i++)
        {
            fputs(args[i], stdout);
            if (args[i + 1])
                printf(" ");
        }
    }
    else
    {
        for (; args[i]; i++)
        {
            printf("%s", args[i]);
            if (args[i + 1])
                printf(" ");
        }
    }
    if (newline)
        printf("\n");

    return 0;
}
