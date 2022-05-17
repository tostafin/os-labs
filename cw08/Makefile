CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread

all: compile test clean

compile:
	$(CC) $(CFLAGS) main.c -o main $(LIBS)

test:
	printf "For numbers method:\n" > Times.txt
	for number in 1 2 4 8 16 ; do \
		printf "\nNumber of threads: $$number\n" >> Times.txt ; \
		./main $$number numbers image.pgm image_neg.pgm >> Times.txt ; \
	done
	printf "\n\nFor block method:\n" >> Times.txt
	for number in 1 2 4 8 16 ; do \
	printf "\nNumber of threads: $$number\n" >> Times.txt ; \
	./main $$number block image.pgm image_neg.pgm >> Times.txt ;\
	done

clean:
	rm -f main
