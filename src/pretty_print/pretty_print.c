#include "pretty_print.h"

int pretty_print(struct ast *ast)
{
    if (!ast)
        return 0;
    int i;
    char *str;
    switch (ast->type)
    {
    case AST_IF:
        printf("if { \ncommand ");
        i = 0;
        pretty_print(ast->op_ast);
        printf("\n}; then { \n");
        pretty_print(ast->left);
        printf("\n}");
        if (ast->right)
        {
            printf(" else { \n");
            pretty_print(ast->right);
            printf("\n}");
        }
        printf("\n");
        break;
    case AST_COMMAND:
        i = 0;
        str = ast->command[i];
        printf("command ");
        while (str != NULL)
        {
            printf("\"%s\" ", str);
            i++;
            str = ast->command[i];
        }
        break;
    default:
        printf("\nYou shouldn't be here\n");
        return 1;
        break;
    }
    return 0;
}
