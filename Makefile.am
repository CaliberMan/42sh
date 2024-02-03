SUBDIRS = src tests

maintainer-clean-local:
	rm -rf aclocal.m4 ar-lib compile configure install-sh autom4te.cache config.log config.status depcomp Makefile Makefile.in missing configure~

debug:
	make check 'LDFLAGS = -fsanitize=address' 'CFLAGS = -g -O0 --coverage'
