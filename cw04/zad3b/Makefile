CC = gcc
CFLAGS = -Wall
OBJS = catcher.o sender.o sigHelper.o

all: test clean

test: $(OBJS)
	@$(CC) $(CFLAGS) catcher.o sigHelper.o -o catcher
	@$(CC) $(CFLAGS) sender.o sigHelper.o -o sender

catcher.o: catcher.c
	$(CC) $(CFLAGS) -c catcher.c

sender.o: sender.c
	$(CC) $(CFLAGS) -c sender.c

sigHelper.o: sigHelper.c sigHelper.h
	$(CC) $(CFLAGS) -c sigHelper.c

clean:
	@rm -f catcher.o sender.o sigHelper.o

