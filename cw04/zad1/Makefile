CC = gcc
CFLAGS = -Wall -Werror

all: test clean

test:
	$(CC) $(CFLAGS) main.c -o main
	./main pending

clean:
	@rm -f main
