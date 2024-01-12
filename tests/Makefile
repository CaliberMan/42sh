CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Wvla
LDFLAGS=-fsanitize=address
LDLIBS=-lcriterion

check: test_pretty_print.o ../src/pretty_print/pretty_print.o ../src/exec_tree/exec_tree.o
	$(CC) -o test_pretty_print test_pretty_print.o ../src/pretty_print/pretty_print.o $(LDFLAGS) $(LDLIBS)
	$(CC) -o test_exec_tree test_exec_tree.o ../src/execute_tree/execute_tree.o $(LDFLAGS) $(LDLIBS)
	./test_pretty_print
	./test_exec_tree

clean:
	$(RM) *.o test_pretty_print test_exec_tree ../src/pretty_print/pretty_print.o
