#include <criterion/criterion.h>
#include <stdlib.h>
#include <criterion/redirect.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../src/exec_tree/exec_tree.h"

FILE *std;

void redirect_all_stdout(void)
{
    cr_redirect_stdout();
    std = cr_get_redirected_stdout();
    cr_redirect_stderr();
}

int error(void)
{
    write(2, "error", 5);
    return (0);
}

Test(Exec_Tree_simple_echo, echo_basic, .init = redirect_all_stdout)
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
    execute_tree(&ast);

    char buf[1028] = {0};
    fseek(std, 0, SEEK_SET);
    fread(buf, 1028, 1, std);
    cr_expect_stdout_eq_str("Hello World\n", "Could not work, got: '%s'", buf);

    fflush(NULL);

    free(ast.command[0]);
    free(ast.command[1]);
    free(ast.command[2]);
}

//Test(Exec_Tree_simple_echo, with_args, .init = redirect_all_stdout)
//{
//    struct ast ast;
//    ast.type = AST_COMMAND;
//    ast.command = malloc(sizeof(char *) * 4);
//    ast.command[0] = malloc(10);
//    ast.command[1] = malloc(21);
//    ast.command[2] = malloc(21);
//    ast.command[3] = malloc(5);
//    strcpy(ast.command[0], "echo");
//    strcpy(ast.command[1], "-n");
//    strcpy(ast.command[2], "Hello World");
//    ast.command[3] = NULL;
//    execute_tree(&ast);
//    cr_expect_stdout_eq_str("Hello World");
//    fflush(NULL);
//    free(ast.command[0]);
//    free(ast.command[1]);
//    free(ast.command[2]);
//}
//
//
//Test(Exec_Tree_simple_echo, chained_echos, .init = redirect_all_stdout)
//{
//    // first ast
//    struct ast ast;
//    ast.type = AST_COMMAND;
//    ast.command = malloc(sizeof(char *) * 4);
//    ast.command[0] = malloc(10);
//    ast.command[1] = malloc(21);
//    ast.command[2] = malloc(21);
//    ast.command[3] = malloc(5);
//    strcpy(ast.command[0], "echo");
//    strcpy(ast.command[1], "-n");
//    strcpy(ast.command[2], "Hello ");
//    ast.command[3] = NULL;
//
//    struct ast ast1;
//    ast1.type = AST_COMMAND;
//    ast1.command = malloc(sizeof(char *) * 3);
//    ast1.command[0] = malloc(10);
//    ast1.command[1] = malloc(21);
//    ast1.command[2] = malloc(5);
//    strcpy(ast1.command[0], "echo");
//    strcpy(ast1.command[1], "World\n");
//    ast1.command[2] = NULL;
//
//    ast.next = &ast1;
//
//    cr_expect_eq(execute_tree(&ast), 0, "Should return 0");
//    cr_expect_stdout_eq_str("Hello World\n", "failed hello world with linked");
//    fflush(NULL);
//    free(ast.command[0]);
//    free(ast.command[1]);
//    free(ast.command[2]);
//    free(ast.command[3]);
//    free(ast1.command[0]);
//    free(ast1.command[1]);
//    free(ast1.command[2]);
//}
