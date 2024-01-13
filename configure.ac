AC_INIT([42sh], [1.0], [david-oritz.calderon@epita.fr])
AM_INIT_AUTOMAKE([subdir-objects] [foreign])

AC_PROG_CC
AX_COMPILER_FLAGS([], [], [], [-Wall -Wextra -Werror -std=c99 -pedantic])

AM_PROG_AR
AC_PROG_RANLIB

AC_CONFIG_FILES([
                 Makefile
                 src/ast/Makefile
                 src/builtins/Makefile
                 src/exec/Makefile
                 src/exec_tree/Makefile
                 src/lexer/Makefile
                 src/parser/Makefile
                 src/pretty_print/Makefile
                 src/Makefile
                 ])

AC_OUTPUT
