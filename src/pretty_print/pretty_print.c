#include <stdio.h>

#include "pretty_print.h"

void tab_print(int x)
{
    for (int i = 0; i < x; i++)
        printf("        ");
}

int pretty_print(struct ast *ast, int x)
{
    if (!ast)
        return 0;
    char *str;
    switch (ast->type)
    {
    case AST_IF:
        tab_print(x);
        printf("if { \n");
        //        printf("if { \ncommand ");
        pretty_print(ast->op_ast, x);
        tab_print(x);
        printf("\n}; then { \n");
        pretty_print(ast->left, x + 1);
        tab_print(x);
        printf("\n}");
        if (ast->right)
        {
            tab_print(x);
            printf(" else { \n");
            pretty_print(ast->right, x + 1);
            tab_print(x);
            printf("\n}");
        }
        pretty_print(ast->next, 0);
        tab_print(x);
        printf("\n");
        break;
    case AST_COMMAND:;
        int i = 0;
        str = ast->command[i];
        //        printf("command ");
        tab_print(x);
        while (str != NULL)
        {
            printf("\"%s\" ", str);
            i++;
            str = ast->command[i];
        }
        pretty_print(ast->next, 0);
        tab_print(x);
        printf("\n");
        break;
    default:
        printf("\nYou shouldn't be here\n");
        return 1;
        break;
    }
    return 0;
}
