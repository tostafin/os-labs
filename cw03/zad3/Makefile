CC = gcc
CFLAGS = -Wall

all: test clean

test:
	@$(CC) $(CFLAGS) main.c -o main
	@./main ./directory string1 3

clean:
	@rm -f main

