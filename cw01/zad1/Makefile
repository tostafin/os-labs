CC = gcc
CFLAGS = -std=c11 -Wall

all: static shared

static:
	$(CC) $(CFLAGS) -c library.c
	ar cr liblibrary.a library.o

shared:
	$(CC) -fPIC -c library.c
	$(CC) -shared -fPIC -o liblibrary.so library.o

clean:
	rm -f *.o *.a *.so
