CC = gcc
CFLAGS = -std=c99 -Wvla -Werror -Wextra -Wall -pedantic

SRCS = parser.c main.c ../ast/ast.c ../lexer/lexer.c
OBJS = parser.o main.o ../ast/ast.o ../lexer/lexer.o
BIN = parser

$(BIN): $(OBJS)

debug: CFLAGS += -g
debug: $(OBJS)

clean:
	$(RM) $(OBJS) $(BIN)
