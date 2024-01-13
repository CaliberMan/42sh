#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "src/ast/ast.h"
#include "src/lexer/lexer.h"
#include "src/parser/parser.h"
#include "src/pretty_print/pretty_print.h"

int is_valid_file(const char *path)
{
    struct stat st;
    if (stat(path, &st) < 0)
        return -1;
    return S_ISREG(st.st_mode);
}

struct lexer *file_to_lexer(char *filename)
{
	char * buffer = 0;
	long length;
	FILE * f = fopen (filename, "rb");
	if (f)
	{
	    fseek (f, 0, SEEK_END);
	    length = ftell (f);
	    fseek (f, 0, SEEK_SET);
	    buffer = malloc(length);
	    if (buffer)
		    fread (buffer, 1, length, f);
	    fclose (f);
	}
	else
  	    return NULL;
	struct lexer *lexer = init_lexer(buffer);
	return lexer;
}

struct lexer *stdin_to_lexer()
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
		if (strcmp("-c", argv[1]))
		{
			fprintf(stderr, "invalid arguments");
			return NULL;
		}
		struct lexer *lexer = init_lexer(argv[2]);
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

int main(int argc, char *argv[])
{
	struct lexer *lexer = create_lexer(argc, argv);
	if (!lexer)
		return 2;
	struct ast *ast;
	enum parser_status ps = parse_input(&ast, lexer);
	lexer_free(lexer);
	if (ps == PARSER_ERROR)
	{
		fprintf(stderr, "error parsing the input");
		free_ast(ast);
		return 2;
	}
	pretty_print(ast);
	free_ast(ast);
	return 0;
}
