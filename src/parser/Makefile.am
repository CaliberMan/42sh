lib_LIBRARIES = libparser.a

libparser_a_SOURCES = \
	input/input.c input/input.h \
	shell_cmd/if/parser_if.c shell_cmd/if/parser_if.h \
	shell_cmd/parser_shell_cmd.c shell_cmd/parser_shell_cmd.h \
	simple_cmd/parser_simple_cmd.c simple_cmd/parser_simple_cmd.h \
	list/parser_list.c list/parser_list.h \
	utils.h
libparser_a_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
libparser_a_CPPFLAGS = -I$(top_srcdir)/src
