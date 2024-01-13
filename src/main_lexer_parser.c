#include "ast.h"
#include "exec_tree.h"
#include "exer.h"
#include "parser.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    // lexing
    struct lexer *lexer = init_lexer(argv[0]);
    if (lexer == NULL)
        printf("lexer: failed to allocate a lexer");

    // parsing
    struct ast *ast;
    enum parser_status status = parse_input(&ast, lexer);
    if (status != PARSER_OK)
        printf("parser: failed to parse the tokens");

    // evaluating
    int result = execute_tree(ast);
    
    if (result)
        printf("execute_tree: failed to evaluate the tree");

    return result;
}
