#include <stdio.h>

#include "parser.h"
#include "pretty_print.h"

int main(int argc, char **argv)
{
    if (argc < 2)
        return 0;

    struct lexer *lexer = init_lexer(argv[1]);
    struct ast *ast;

    enum parser_status status = parse_input(&ast, lexer);

    struct ast *tmp = ast->op_ast;
    if (!tmp)
        return 1;

    if (status != PARSER_OK)
    {
        printf("parser: failed");
        return 1;
    }

    pretty_print(ast);
    //    pretty_print(ast->next);
    return 0;
}
