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
        printf("}; then { \n");
        pretty_print(ast->data.ast_if.then_body, x + 1);
        tab_print(x);
        printf("}\n");
        if (ast->data.ast_if.else_body)
        {
            tab_print(x);
            printf(" else { \n");
            pretty_print(ast->data.ast_if.else_body, x + 1);
            tab_print(x);
            printf("}\n");
        }
        tab_print(x);
        printf("\n");
        break;
    case AST_CMD:;
        int i = 0;
        str = ast->data.ast_cmd.words[i];
        tab_print(x);
        printf("CMD:");
        while (str != NULL)
        {
            printf("\"%s\" ", str);
            i++;
            str = ast->data.ast_cmd.words[i];
        }
        tab_print(x);
        printf("\n");
        break;
    case AST_LOOP:
        if (ast->data.ast_loop.type == WHILE_LOOP)
        {
            tab_print(x);
            printf("while {\n");
            pretty_print(ast->data.ast_loop.cond, x + 1);
            tab_print(x);
            printf("}\n");
        }
        tab_print(x);
        printf("{\n");
        pretty_print(ast->data.ast_loop.then_body, x + 1);
        tab_print(x);
        printf("}\n");
        if (ast->data.ast_loop.type == UNTIL_LOOP)
        {
            tab_print(x);
            printf("until {\n");
            pretty_print(ast->data.ast_loop.cond, x + 1);
            tab_print(x);
            printf("}\n");
        }
        break;
    case AST_NOT:
        tab_print(x);
        printf("NOT {\n");
        pretty_print(ast->data.ast_not.child, x + 1);
        tab_print(x);
        printf("}\n");
        break;
    case AST_LIST:
        for (size_t i = 0; i < ast->data.ast_list.nb_nodes; i++)
            pretty_print(ast->data.ast_list.list[i], x);
        break;
    case AST_OPERATOR:
        tab_print(x);
        printf("{\n");
        pretty_print(ast->data.ast_operator.left, x + 1);
        tab_print(x);
        printf("}\n");
        tab_print(x);
        if (ast->data.ast_operator.type == OP_OR)
            printf("or\n");
        else
            printf("and\n");
        tab_print(x);
        printf("{\n");
        pretty_print(ast->data.ast_operator.right, x + 1);
        tab_print(x);
        printf("}\n");
        break;
    case AST_VARIABLE:
        tab_print(x);
        printf("VAR { ");
        printf("%s = ", ast->data.ast_variable.name);
        pretty_print(ast->data.ast_variable.value, x);
        printf(" }\n");
        break;
    default:
        printf("\nYou shouldn't be here\n");
        return 1;
        break;
    }
    return 0;
}
