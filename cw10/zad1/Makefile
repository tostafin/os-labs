CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread

all: compile run_server run_clients clean

compile:
	$(CC) $(CFLAGS) server.c -o server
	$(CC) $(CFLAGS) client.c -o client

run_server:
	valgrind ./server 32768 serverSocket
	#gnome-terminal -- ./server 32768 serverSocket

run_clients:
	valgrind ./client client1 network 127.0.0.1:32768
	#gnome-terminal -- ./client client1 network 127.0.0.1:32768
	#gnome-terminal -- ./client client2 network 127.0.0.1:32768

clean:
	rm -f server client
