#ifndef AST_PIPE_H
#define AST_PIPE_H

struct ast_pipe
{
    struct ast *left_arg; // left argument
    struct ast *right_arg; // right argument
};

#endif /* ! AST_PIPE_H */
