CC = gcc
CFLAGS = -Wall

all: test clean

test:
	@$(CC) $(CFLAGS) main.c -o main
	@./main ./file.txt

clean:
	@rm -f main

