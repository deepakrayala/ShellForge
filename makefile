CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
LIBS = -lreadline

SRC = src/history.c src/lexer.c src/main.c src/token.c src/builtin.c src/executor.c src/pipeline.c

shellforge: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o shellforge

clean:
	rm -f shellforge
