lib_LIBRARIES = libbuiltins.a

libbuiltins_a_SOURCES = builtins.c builtins.h
libbuiltins_a_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
libbuiltins_a_CPPFLAGS = -I$(top_srcdir)/src

clean-local:
	rm -rf .deps

maintainer-clean-local:
	rm -rf Makefile Makefile.in
