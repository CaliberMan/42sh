bin_PROGRAMS = 42sh
42sh_SOURCES = main.c
42sh_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
42sh_CPPFLAGS = -I%D%

42sh_LDADD = \
	ast/libast.a \
	parser/libparser.a \
	lexer/liblexer.a \
	exec_tree/libexec_tree.a \
	builtins/libbuiltins.a \
	utils/libutils.a \
	exec/libexec.a

SUBDIRS = utils ast builtins exec exec_tree lexer parser

clean-local:
	rm -rf .deps

maintainer-clean-local:
	rm -rf Makefile Makefile.in

