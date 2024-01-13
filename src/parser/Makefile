CC = gcc
CFLAGS = -std=c99 -Wvla -Werror -Wextra -Wall -pedantic
CPPFLAGS = -Isrc -Isrc/ast -Isrc/parser -Isrc/lexer -Isrc/pretty_print -Itests

OBJS = parser.o \
	   ../ast/ast.o \
	   ../lexer/lexer.o \
	   ../pretty_print/pretty_print.o \
	   ../../tests/parser/test.o

BIN = parser

$(BIN): $(OBJS)
	$(CC) -o $(BIN) $(OBJS)

debug: CFLAGS += -g
debug: $(BIN)

clean:
	$(RM) $(OBJS) $(BIN)
