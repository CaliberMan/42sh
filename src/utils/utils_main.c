#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

#include "utils_main.h"

int is_valid_file(const char *path)
{
    struct stat st;
    if (stat(path, &st) < 0)
        return -1;
    return S_ISREG(st.st_mode);
}

struct lexer *file_to_lexer(char *filename)
{
    char *buffer;
    long length;
    FILE *f = fopen(filename, "rb");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = calloc(length + 1, sizeof(char));
        if (buffer)
            fread(buffer, 1, length, f);
        fclose(f);
    }
    else
    {
        fprintf(stderr, "no file????? BOMBOCLAT!!!\n");
        return NULL;
    }
    struct lexer *lexer = init_lexer(buffer);
    return lexer;
}

struct lexer *stdin_to_lexer(void)
{
    char *buffer = calloc(100, sizeof(char));
    int len = 0;
    int capacity = 100;
    char c = getchar();
    while (c != EOF)
    {
        buffer[len] = c;
        len++;
        if (len == capacity)
        {
            buffer = realloc(buffer, sizeof(char) * capacity * 2);
            capacity *= 2;
        }
        c = getchar();
    }
    buffer[len] = 0;
    struct lexer *lexer = init_lexer(buffer);
    return lexer;
}

struct lexer *create_lexer(int argc, char *argv[])
{
    if (argc > 3)
    {
        fprintf(stderr, "Invalid number of arguments");
        return NULL;
    }
    if (argc == 3)
    {
        if (strcmp("-p", argv[2]) == 0)
        {
            struct lexer *lexer = file_to_lexer(argv[1]);
            return lexer;
        }
        if (strcmp("-c", argv[1]) && strcmp("-p", argv[1]))
        {
            fprintf(stderr, "invalid arguments");
            return NULL;
        }
        
	char *buffer = calloc(strlen(argv[2]) + 1, sizeof(char));
	for (int i = 0; argv[2][i]; i++)
		buffer[i] = argv[2][i];
        struct lexer *lexer = init_lexer(buffer);
        return lexer;
    }
    else if (argc == 2)
    {
        struct lexer *lexer = file_to_lexer(argv[1]);
        return lexer;
    }
    else
    {
        struct lexer *lexer = stdin_to_lexer();
        return lexer;
    }
}
