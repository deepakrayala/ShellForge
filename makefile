CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
LIBS = -lreadline

TARGET = shellforge

SRC = src/history.c src/lexer.c src/main.c src/token.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all

.PHONY: all run clean rebuild
