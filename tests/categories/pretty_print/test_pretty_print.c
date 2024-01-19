#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../src/pretty_print/pretty_print.h"

void redirect_all_stdout(void)
{
    cr_redirect_stdout();
    cr_redirect_stderr();
}

int error(void)
{
    write(2, "error", 5);
    return (0);
}

Test(Pretty_Print_EasyTest, Empty, .init = redirect_all_stdout)
{
    cr_expect_eq(pretty_print(NULL), 0, "Should return 0 and print nothing");
    fflush(NULL);
    cr_expect_stdout_eq_str("", "Printed something when shouldn't have");
}
Test(Pretty_Print_EasyTest, SecondTest, .init = redirect_all_stdout)
{
    struct ast ast;
    ast.type = AST_COMMAND;
    ast.command = malloc(sizeof(char *) * 3);
    ast.command[0] = malloc(10);
    ast.command[1] = malloc(21);
    ast.command[2] = malloc(5);
    strcpy(ast.command[0], "echo");
    strcpy(ast.command[1], "Hello World");
    ast.command[2] = NULL;
    cr_expect_eq(pretty_print(&ast), 0, "Should return 0");
    fflush(NULL);
    cr_expect_stdout_eq_str("command \"echo\" \"Hello World\" ",
                            "Wrong format output for echo hello world");
    free(ast.command[0]);
    free(ast.command[1]);
    free(ast.command[2]);
}
