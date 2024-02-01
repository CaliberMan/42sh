#ifndef UTILS_MAIN_H
#define UTILS_MAIN_H

#include "../lexer/lexer.h"

struct lexer *file_to_lexer(char *filename);
struct lexer *stdin_to_lexer(void);
struct lexer *create_lexer(int argc, char *argv[]);

#endif /* ! UTILS_MAIN_H */
