CC = gcc
CFLAGS = -std=c11 -Wall -Werror -g

all: test clean

test:
	@$(CC) $(CFLAGS) main.c -o main
	@./main 7

clean:
	@rm -f main
