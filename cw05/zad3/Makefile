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
	@rm -f pipe
	@mkfifo pipe
	@make test_n_5 --no-print-directory
	@make test_n_500 --no-print-directory
	@make test_n_5000 --no-print-directory

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
	@truncate -s 0 ./files/writeFile.txt
	./runOneConsumerFiveProducers
	@make run_tester_n_5 --no-print-directory

test_n_5:
	@printf "Many producers, one consumer:\n"
	@truncate -s 0 ./files/writeFile.txt
	for number in 1 2 3 4 5 ; do \
		(./producer ./pipe $$number ./files/N5/file$$number.txt $(N1) &) ; \
	done
	./consumer ./pipe ./files/writeFile.txt $(N1)
	@make run_tester_n_5 --no-print-directory
	
	@truncate -s 0 ./files/writeFile.txt
	@printf "One producer, many consumers:\n"
	for number in 1 2 3 4 5 ; do \
		(./consumer ./pipe ./files/writeFile.txt $(N1) &) ; \
	done
	./producer ./pipe 1 ./files/N5/file1.txt $(N1)
	@make run_tester_n_5_one_line --no-print-directory
	
	@truncate -s 0 ./files/writeFile.txt
	@printf "Many producer, many consumers:\n"
	for number in 1 2 3 4 5 ; do \
		(./producer ./pipe $$number ./files/N5/file$$number.txt $(N1) &) ; \
	done
	
	for number in 1 2 3 4 ; do \
		(./consumer ./pipe ./files/writeFile.txt $(N1) &) ; \
	done
	
	./consumer ./pipe ./files/writeFile.txt $(N1)
	@make run_tester_n_5 --no-print-directory

test_n_500:
	@printf "Many producers, one consumer:\n"
	@truncate -s 0 ./files/writeFile.txt
	for number in 1 2 3 4 5 ; do \
		(./producer ./pipe $$number ./files/N500/file$$number.txt $(N2) &) ; \
	done
	./consumer ./pipe ./files/writeFile.txt $(N2)
	@make run_tester_n_500 --no-print-directory
	
	@truncate -s 0 ./files/writeFile.txt
	@printf "One producer, many consumers:\n"
	for number in 1 2 3 4 5 ; do \
		(./consumer ./pipe ./files/writeFile.txt $(N2) &) ; \
	done
	./producer ./pipe 1 ./files/N500/file1.txt $(N2)
	@make run_tester_n_500_one_line --no-print-directory
	
	@truncate -s 0 ./files/writeFile.txt
	@printf "Many producer, many consumers:\n"
	for number in 1 2 3 4 5 ; do \
		(./producer ./pipe $$number ./files/N500/file$$number.txt $(N2) &) ; \
	done
	
	for number in 1 2 3 4 ; do \
		(./consumer ./pipe ./files/writeFile.txt $(N2) &) ; \
	done
	
	./consumer ./pipe ./files/writeFile.txt $(N2)
	@make run_tester_n_500 --no-print-directory

test_n_5000:
	@printf "Many producers, one consumer:\n"
	@truncate -s 0 ./files/writeFile.txt
	for number in 1 2 3 4 5 ; do \
		(./producer ./pipe $$number ./files/N5000/file$$number.txt $(N3) &) ; \
	done
	./consumer ./pipe ./files/writeFile.txt $(N3)
	@make run_tester_n_5000 --no-print-directory
	
	@truncate -s 0 ./files/writeFile.txt
	@printf "One producer, many consumers:\n"
	for number in 1 2 3 4 5 ; do \
		(./consumer ./pipe ./files/writeFile.txt $(N3) &) ; \
	done
	./producer ./pipe 1 ./files/N5000/file1.txt $(N3)
	@make run_tester_n_5000_one_line --no-print-directory
	
	@truncate -s 0 ./files/writeFile.txt
	@printf "Many producer, many consumers:\n"
	for number in 1 2 3 4 5 ; do \
		(./producer ./pipe $$number ./files/N5000/file$$number.txt $(N3) &) ; \
	done
	
	for number in 1 2 3 4 ; do \
		(./consumer ./pipe ./files/writeFile.txt $(N3) &) ; \
	done
	
	./consumer ./pipe ./files/writeFile.txt $(N3)
	@make run_tester_n_5000 --no-print-directory

run_tester_n_5_one_line:
	@printf "Running tester:\n"
	./tester ./files/N5/file1.txt ./files/writeFile.txt 1
	@printf "\n"

run_tester_n_5:
	@printf "Running tester:\n"
	for number in 1 2 3 4 5 ; do \
		(./tester ./files/N5/file$$number.txt ./files/writeFile.txt $$number) ; \
	done
	@printf "\n"

run_tester_n_500_one_line:
	@printf "Running tester:\n"
	./tester ./files/N500/file1.txt ./files/writeFile.txt 1
	@printf "\n"

run_tester_n_500:
	@printf "Running tester:\n"
	for number in 1 2 3 4 5 ; do \
		(./tester ./files/N500/file$$number.txt ./files/writeFile.txt $$number) ; \
	done
	@printf "\n"

run_tester_n_5000_one_line:
	@printf "Running tester:\n"
	./tester ./files/N5000/file1.txt ./files/writeFile.txt 1
	@printf "\n"

run_tester_n_5000:
	@printf "Running tester:\n"
	for number in 1 2 3 4 5 ; do \
		(./tester ./files/N5000/file$$number.txt ./files/writeFile.txt $$number) ; \
	done
	@printf "\n"

clean:
	@rm -f producer producer.o consumer consumer.o common.o runOneConsumerFiveProducers.o runOneConsumerFiveProducers tester pipe

