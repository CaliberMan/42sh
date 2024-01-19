lib_LIBRARIES = libexec.a

libexec_a_SOURCES = exec.c exec.h
libexec_a_CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic
libexec_a_CPPFLAGS = -I$(top_srcdir)/src

clean-local:
	rm -rf .deps

maintainer-clean-local:
	rm -rf Makefile Makefile.in

