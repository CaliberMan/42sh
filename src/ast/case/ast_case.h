#ifndef AST_CASE_H
#define AST_CASE_H

struct ast_case
{
    char *expr;
    struct ast *cases_list; // ast_list of ast_pattern
};

struct ast_pattern
{
    struct ast *pattern; // ast_list
    struct ast *statement;
};

/**
 * case EXPR in
 *  PATTERN_1)
 *      STATEMENT
 *  PATTERN_2)
 *      STATEMENT =>                ast_case -> expr
 *  *)                                 ↓
 *      STATEMENT                   cases_list ...
 *  esac                            ↙         ↘
 *                        ast_pattern         ast_pattern
 *                          ↙       ↘           ↙      ↘
 *                      pattern_1 statement pattern_2  statement
 */

void free_pattern(struct ast_pattern *ast);

void free_case(struct ast_case *ast);

#endif /* ! AST_CASE_H */
