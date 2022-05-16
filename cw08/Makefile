CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread

all: compile run_v

compile:
	$(CC) $(CFLAGS) main.c -o main $(LIBS)

run:
	./main 3 block image.pgm image_neg.pgm
	./main 3 block image1.pgm image1_neg.pgm

run_v:
	valgrind -s --leak-check=full --show-leak-kinds=all ./main 3 block image.pgm image_neg.pgm
	valgrind -s --leak-check=full --show-leak-kinds=all ./main 3 block image1.pgm image1_neg.pgm


clean:
	rm -f main
