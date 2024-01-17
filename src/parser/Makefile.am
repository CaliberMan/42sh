lib_LIBRARIES = libparser.a

libparser_a_SOURCES = parser.c parser.h
libparser_a_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
libparser_a_CPPFLAGS = -I$(top_srcdir)/src

clean-local:
	rm -rf .deps

maintainer-clean-local:
	rm -rf Makefile Makefile.in
