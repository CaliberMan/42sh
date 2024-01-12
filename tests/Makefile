CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Wvla
LDFLAGS=-fsanitize=address
LDLIBS=-lcriterion

check: test_pretty_print.o ../src/pretty_print/pretty_print.o
	$(CC) -o test_pretty_print test_pretty_print.o ../src/pretty_print/pretty_print.o $(LDFLAGS) $(LDLIBS)
	./test_pretty_print

clean:
	$(RM) *.o test_pretty_print ../src/pretty_print/pretty_print.o
