#include "builtins.h"

int b_true(void)
{
    return 0;
}

int b_false(void)
{
    return 1;
}

int b_echo(char **args)
{
    size_t i = 1;
    int newline = 1;
    int interpreted = 0;
    for (; args[i]; i++)
    {
        if (strcmp("-n", args[i]) == 0)
            newline = 0;
        // use puts
        else if (strcmp("-E", args[i]) == 0)
            interpreted = 0;
        else if (strcmp("-e", args[i]) == 0)
            interpreted = 1;
        else
            break;
    }
    if (interpreted)
    {
        for (; args[i]; i++)
        {
            puts(args[i]);
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
