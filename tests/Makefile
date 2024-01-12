CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Wvla -g
LDFLAGS=-fsanitize=address
LDLIBS=-lcriterion

check: test_pretty_print.o ../src/pretty_print/pretty_print.o ../src/exec_tree/exec_tree.o test_exec_tree.o ../src/exec/exec.o ../src/builtins/builtins.o
	$(CC) -o test_pretty_print test_pretty_print.o ../src/pretty_print/pretty_print.o $(LDFLAGS) $(LDLIBS)
	$(CC) -o test_exec_tree test_exec_tree.o ../src/builtins/builtins.o ../src/exec/exec.o ../src/exec_tree/exec_tree.o $(LDFLAGS) $(LDLIBS)
	./test_pretty_print
	./test_exec_tree

clean:
	$(RM) *.o test_pretty_print test_exec_tree ../src/pretty_print/pretty_print.o ../src/builtins/builtins.o ../src/exec/exec.o ../src/exec_tree/exec_tree.o
