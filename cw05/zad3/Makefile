CC = gcc
CFLAGS = -Wall
OBJS = producer.o consumer.o common.o

all: clean test

test: $(OBJS)
	@$(CC) $(CFLAGS) producer.o common.o -o producer
	@$(CC) $(CFLAGS) consumer.c common.o -o consumer
	@$(CC) $(CFLAGS) tester.c common.o -o tester
	@mkfifo pipe
	make test_producer
	make test_consumer
	make run_tester

producer.o: producer.c
	@$(CC) $(CFLAGS) -c producer.c

consumer.o: consumer.c
	@$(CC) $(CFLAGS) -c consumer.c

common.o: common.c common.h
	@$(CC) $(CFLAGS) -c common.c

test_producer:
	for number in 1 2 3 4 5 ; do \
		(./producer ./pipe $$number ./files/file$$number.txt 5 &) ; \
	done

test_consumer:
	./consumer ./pipe ./files/writeFile.txt 5

run_tester:
	for number in 1 2 3 4 5 ; do \
		(./tester ./files/file$$number.txt ./files/writeFile.txt $$number) ; \
	done

clean:
	@rm -f producer producer.o consumer consumer.o common.o pipe

