CC = gcc
CFLAGS = -Wall
OBJS = producer.o consumer.o common.o runOneConsumerFiveProducers.o
N1 = 5
N2 = 500
N3 = 5000

all: test

test: $(OBJS)
	@make compile_producer_and_consumer --no-print-directory
	@printf "Running five producers and one consumer:\n"
	@make test_five_producers_one_consumer --no-print-directory

compile_producer_and_consumer: $(OBJS)
	@$(CC) $(CFLAGS) producer.o common.o -o producer
	@$(CC) $(CFLAGS) consumer.c common.o -o consumer
	@$(CC) $(CFLAGS) runOneConsumerFiveProducers.c common.o -o runOneConsumerFiveProducers
	@$(CC) $(CFLAGS) tester.c common.o -o tester

producer.o: producer.c
	@$(CC) $(CFLAGS) -c producer.c

consumer.o: consumer.c
	@$(CC) $(CFLAGS) -c consumer.c

runOneConsumerFiveProducers.o: runOneConsumerFiveProducers.c
	@$(CC) $(CFLAGS) -c runOneConsumerFiveProducers.c

tester.o: tester.c
	@$(CC) $(CFLAGS) -c tester.c

common.o: common.c common.h
	@$(CC) $(CFLAGS) -c common.c

test_five_producers_one_consumer:
	./runOneConsumerFiveProducers
	@make run_tester --no-print-directory

test_producer:
	for number in 1 2 3 4 5 ; do \
		(./producer ./pipe $$number ./files/N=5/file$$number.txt $(N1) &) ; \
	done

test_consumer:
	./consumer ./pipe ./files/writeFile.txt $(N1)

run_tester:
	@printf "Running tester:\n"
	for number in 1 2 3 4 5 ; do \
		(./tester ./files/N5/file$$number.txt ./files/writeFile.txt $$number) ; \
	done
	@printf "\n"

clean:
	@rm -f producer producer.o consumer consumer.o common.o runOneConsumerFiveProducers.o runOneConsumerFiveProducers tester pipe

