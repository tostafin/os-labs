CC = gcc
CFLAGS = -Wall

all: test clean

test:
	@$(CC) -c sigHelper.c
	@ar cr libsigHelper.a sigHelper.o
	@$(CC) $(CFLAGS) main.c -o main -L. -l:libsigHelper.a
	@$(CC) $(CFLAGS) execTest.c -o execTest -L. -l:libsigHelper.a
	@./main ignore
	@./main handler
	@./main mask
	@./main pending

clean:
	@rm -f main execTest libsigHelper.a sigHelper.o

