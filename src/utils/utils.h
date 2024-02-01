#ifndef UTILS_H
#define UTILS_H

#include "../lexer/lexer.h"

struct lexer *file_to_lexer(char *filename);

struct lexer *stdin_to_lexer(void);

struct lexer *create_lexer(int argc, char *argv[]);

#endif /* ! UTILS_H */
