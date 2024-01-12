CC = gcc
CFLAGS = -std=c99 -Wvla -Werror -Wextra -Wall -pedantic -g
CPPFLAGS = -Isrc -Isrc/ast -Isrc/parser -Isrc/lexer -Isrc/pretty_print

OBJS = src/parser/parser.o \
	   src/ast/ast.o \
	   src/lexer/lexer.o \
	   src/pretty_print/pretty_print.o \
	   src/test.o

BIN = parser

$(BIN): $(OBJS)
	$(CC) -o $(BIN) $(OBJS)

debug: CFLAGS += -g
debug: $(OBJS)

clean:
	$(RM) $(OBJS) $(BIN)
