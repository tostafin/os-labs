CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread

#all: clean compile run_server run_clients
all: clean compile

compile: common.o
	$(CC) $(CFLAGS) $< server.c -o server
	$(CC) $(CFLAGS) $< client.c -o client

common.o: common.c
	$(CC) $(CFLAGS) -c $<

run_server:
	#valgrind ./server 32768 serverSocket
	gnome-terminal -- ./server 32768 serverSocket
	#./server 32768 serverSocket

run_clients:
	#valgrind ./client client1 network 127.0.0.1:32768
	gnome-terminal -- ./client client1 local serverSocket
	gnome-terminal -- ./client client2 local serverSocket
	#./client client1 network 127.0.0.1:32768
	#./client client2 network 127.0.0.1:32768

run_server1:
	./server 32768 serverSocket

run_clients1:
	./client client1 local serverSocket

run_clients2:
	./client client2 local serverSocket

clean:
	rm -f server client
