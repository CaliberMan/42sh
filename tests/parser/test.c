#include "parser.h"
#include "pretty_print.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc < 2)
        return 0;

    struct lexer *lexer = init_lexer(argv[1]);
    struct ast *ast;

    enum parser_status status = parse_input(&ast, lexer);
    if (status != PARSER_OK)
    {
        printf("parser: failed");
        return 1;
    }

    pretty_print(ast);
    return 0;
}
