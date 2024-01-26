lib_LIBRARIES = libast.a

libast_a_SOURCES = ast.c ast.h \
				   cmd/ast_cmd.c cmd/ast_cmd.h \
				   if/ast_if.h if/ast_if.c \
				   pipe/ast_pipe.h pipe/ast_pipe.c \
				   redir/ast_redir.h redir/ast_redir.c \
				   loop/ast_loop.h loop/ast_loop.c \
				   operator/ast_operator.h operator/ast_operator.c \
				   not/ast_not.h not/ast_not.c \
				   variable/ast_variable.h variable/ast_variable.c

libast_a_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
libast_a_CPPFLAGS = -I$(top_srcdir)/src

clean-local:
	rm -rf .deps cmd/.deps if/.deps pipe/.deps redir/.deps loop/.deps \
	operator/.deps not/.deps variable/.deps

maintainer-clean-local:
	rm -rf Makefile Makefile.in
