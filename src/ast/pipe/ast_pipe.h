#ifndef AST_PIPE_H
#define AST_PIPE_H

struct ast_pipe
{
    struct ast *left_arg; // left argument
    struct ast *right_arg; // right argument
};

void free_pipe(struct ast_pipe *ast);

#endif /* ! AST_PIPE_H */
