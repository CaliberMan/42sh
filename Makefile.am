bin_PROGRAMS = 42sh
42sh_SOURCES = src/main.c \
      src/ast/ast.c src/ast/ast.h \
      src/builtins/builtins.c src/builtins/builtins.h \
      src/exec/exec.c src/exec/exec.h \
      src/exec_tree/exec_tree.c src/exec_tree/exec_tree.h \
      src/lexer/lexer.c src/lexer/lexer.h \
      src/parser/parser.c src/parser/parser.h \
      src/pretty_print/pretty_print.c src/pretty_print/pretty_print.h

42sh_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
42sh_CPPFLAGS = -I$(top_srcdir)/src

42sh_LDADD = \
	$(top_builddir)/src/ast/libast.a
	$(top_builddir)/src/builtins/libbuiltins.a
	$(top_builddir)/src/exec/libexec.a
	$(top_builddir)/src/exec_tree/libexec_tree.a
	$(top_builddir)/src/lexer/liblexer.a
	$(top_builddir)/src/parser/libparser.a
	$(top_builddir)/src/pretty_print/libpretty_print.a

SUBDIRS = src/ast src/builtins src/exec src/exec_tree src/lexer src/parser src/pretty_print
