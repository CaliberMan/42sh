#include "pretty_print.h"

#include <stdio.h>

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
        pretty_print(ast->data.ast_if.cond, x + 1);
        tab_print(x);
        printf("\n}; then { \n");
        pretty_print(ast->data.ast_if.then_body, x + 1);
        tab_print(x);
        printf("\n}");
        if (ast->data.ast_if.else_body)
        {
            tab_print(x);
            printf(" else { \n");
            pretty_print(ast->data.ast_if.else_body, x + 1);
            tab_print(x);
            printf("\n}");
        }
        //        pretty_print(ast->next, 0);
        tab_print(x);
        printf("\n");
        break;
    case AST_CMD:;
        int i = 0;
        str = ast->data.ast_cmd.words[i];
        tab_print(x);
        while (str != NULL)
        {
            printf("\"%s\" ", str);
            i++;
            str = ast->data.ast_cmd.words[i];
        }
        //      pretty_print(ast->next, 0);
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
