lib_LIBRARIES = libexec_tree.a

libexec_tree_a_SOURCES = exec_tree.c exec_tree.h \
			 variables/variable.c variables/variable.h \
			 variables/variable_new.c variables/variable_new.h \
			 var_utils/var_utils.c var_utils/var_utils.h

libexec_tree_a_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
libexec_tree_a_CPPFLAGS = -I$(top_srcdir)/src

clean-local:
	rm -rf .deps variables/.deps var_utils/.deps

maintainer-clean-local:
	rm -rf Makefile Makefile.in
