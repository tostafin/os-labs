CC = gcc
CFLAGS = -Wall
OBJS = server.o client.o common.o

all: ipcrm compile clean

ipcrm:
	@ipcrm --all=msg

compile: $(OBJS)
	@$(CC) $(CFLAGS) server.o common.o -o server
	@$(CC) $(CFLAGS) client.o common.o -o client

server.o: server.c
	@$(CC) $(CFLAGS) -c server.c

client.o: client.c
	@$(CC) $(CFLAGS) -c client.c

common.o: common.c
	@$(CC) $(CFLAGS) -c common.c

clean:
	@rm -rf $(OBJS)
