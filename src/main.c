#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "ast/ast.h"
#include "exec/exec.h"
#include "exec_tree/exec_tree.h"
#include "exec_tree/variables/variable.h"
#include "lexer/lexer.h"
#include "pretty_print/pretty_print.h"
#include "parser/input/input.h"
#include "unistd.h"
#include "utils/utils_main.h"



int main(int argc, char *argv[])
{
    struct lexer *lexer = create_lexer(argc, argv);
    if (!lexer)
        return 2;
    if (lexer->input[0] == 0)
    {
        lexer_free(lexer);
        return 0;
    }
    struct ast *ast;
    enum parser_status ps = parse_input(&ast, lexer);
    if (ps == PARSER_ERROR)
    {
        fprintf(stderr, "error parsing the input");
        free_ast(ast);
        lexer_free(lexer);
        return 2;
    }
    init_variables();
    int default_fds[2] = { STDIN_FILENO, STDOUT_FILENO };
    struct exec_arguments command;

    // copy the contents of pipe into the struct
    memcpy(command.pipe_fds, default_fds, sizeof(default_fds));
    struct ret_msg ans;
    ans.type = VAL;
    ans.value = 0;
    if (strcmp("-p", argv[1]) == 0 || (argc == 3 &&  strcmp("-p", argv[2]) == 0))
        pretty_print(ast, 0);
    else
        ans = execute_tree(ast, command);
    lexer_free(lexer);
    free_ast(ast);
    free_list_variables();
    if (ans.value == -1)
    {
        fprintf(stderr, "execute_tree error");
        return 1;
    }
    return ans.value;
}
