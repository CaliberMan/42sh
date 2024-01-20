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

clean-local:
	rm -rf .deps input/.deps shell_cmd/.deps shell_cmd/if/.deps \
		simple_cmd/.deps list/.deps

maintainer-clean-local:
	rm -rf Makefile Makefile.in \
		input/Makefile.in input/Makefile \
		shell_cmd/if/Makefile.in shell_cmd/if/Makefile \
		shell_cmd/Makefile.in shell_cmd/Makefile \
		simple_cmd/Makefile.in simple_cmd/Makefile \
		list/Makefile.in list/Makefile

