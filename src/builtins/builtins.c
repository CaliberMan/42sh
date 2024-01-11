#include <string.h>
#include <stdio.h>

int true(void)
{
    return 0;
}

int false(void)
{
    return 0;
}

void echo(char **args)
{
    for (size_t i = 0; args[i]; i++)
        fprintf(stdout,"%s", args[i]);
}
