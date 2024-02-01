lib_LIBRARIES = libutils.a

libutils_a_SOURCES = utils.c utils.h

libutils_a_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
libutils_a_CPPFLAGS = -I$(top_srcdir)/src

clean-local:
	rm -rf .deps

maintainer-clean-local:
	rm -rf Makefile Makefile.in
