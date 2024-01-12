#include "exec_tree.h"

static int check_builtins(char **command)
{
    if (strcmp(command[0], "echo") == 0)
        return b_echo(command);
    else if (strcmp(command[0], "true") == 0)
        return b_true();
    else if (strcmp(command[0], "false") == 0)
        return b_false();
    else
        return exec(command);
}

int execute_tree(struct ast *ast)
{
    if (!ast)
        return 0;
    int ans;
    switch (ast->type)
    {
    case AST_IF:
        ans = execute_tree(ast->op_ast);
        if (ans == 0)
            ans = execute_tree(ast->left);
        else if (ans == 1)
            ans = execute_tree(ast->right);
        else
            ans = -1;
        if (ast->next != NULL)
            execute_tree(ast->next);
        return ans;
        break;
    case AST_COMMAND:
        ans = check_builtins(ast->command);
        if (ast->next != NULL)
            execute_tree(ast->next);
        return ans;
        break;
    default:
        return -1;
        break;
    }
}

//int main(void)
//{
//    struct ast ast;
//    ast.type = AST_IF;
//    ast.next = NULL;
//
//    struct ast unpog;
//    unpog.type = AST_COMMAND;
//    unpog.command = malloc(sizeof(char *) * 3);
//    unpog.command[0] = malloc(10);
//    unpog.command[1] = malloc(21);
//    unpog.command[2] = malloc(5);
//    strcpy(unpog.command[0], "echo");
//    strcpy(unpog.command[1], "NOTOKAY");
//    unpog.command[2] = NULL;
//    unpog.next = NULL;
//
//    struct ast ast1;
//    ast1.type = AST_COMMAND;
//    ast1.command = malloc(sizeof(char *) * 3);
//    ast1.command[0] = malloc(10);
//    ast1.command[1] = malloc(21);
//    ast1.command[2] = malloc(5);
//    strcpy(ast1.command[0], "echo");
//    strcpy(ast1.command[1], "OKAY");
//    ast1.command[2] = NULL;
//    ast1.next = NULL;
//
//    struct ast ast2;
//    ast2.type = AST_COMMAND;
//    ast2.command = malloc(sizeof(char *) * 3);
//    ast2.command[0] = malloc(10);
//    ast2.command[1] = malloc(10);
//    strcpy(ast2.command[0], "echo");
//    strcpy(ast2.command[1], "TEST");
//    ast2.command[2] = NULL;
//    ast2.next = NULL;
//
//    ast.left = &ast1;
//    ast.right = &unpog;
//    ast.op_ast = &ast2;
//
//    execute_tree(&ast);
//
//    return 0;
//}
