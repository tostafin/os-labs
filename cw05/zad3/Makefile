CC = gcc
CFLAGS = -Wall

all: clean test

test:
	@$(CC) $(CFLAGS) producer.c -o producer
	@$(CC) $(CFLAGS) consumer.c -o consumer
	@mkfifo pipe
	make test_producer
	make test_consumer

test_producer:
	for number in 1 2 3 4 5 ; do \
		(./producer ./pipe $$number ./files/file$$number.txt 5 &) ; \
	done

test_consumer:
	./consumer ./pipe ./files/writeFile.txt 5

clean:
	@rm -f producer consumer pipe

