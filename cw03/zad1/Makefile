CC = gcc
CFLAGS = -Wall

all: test clean

test:
	@$(CC) $(CFLAGS) main.c -o main
	@./main 10

clean:
	@rm -f main
