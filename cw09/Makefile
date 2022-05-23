CC = gcc
CFLAGS = -Wall
LIBS = -pthread

all: compile run

compile:
	$(CC) $(CFLAGS) main.c -o main

run:
	valgrind -s --leak-check=full --show-leak-kinds=all ./main

clean:
	rm -f main
