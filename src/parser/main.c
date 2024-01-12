#include "parser.h"
#include "../pretty_print/pretty_print.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    struct lexer *lexer = init_lexer("echo bar;");
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
