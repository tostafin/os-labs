CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread

all: compile run

compile:
	$(CC) $(CFLAGS) main.c -o main $(LIBS)

run:
	#./main 3 block image.pgm image_neg.pgm
	#./main 3 block image1.pgm image1_neg.pgm
	./main 16 numbers image2.pgm image2_neg.pgm

run_v:
	#valgrind -s --leak-check=full --show-leak-kinds=all ./main 4 numbers image.pgm image_neg.pgm
	#valgrind -s --leak-check=full --show-leak-kinds=all ./main 16 block image.pgm image_neg.pgm
	#valgrind -s --leak-check=full --show-leak-kinds=all ./main 16 numbers image1.pgm image1_neg.pgm
	#valgrind -s --leak-check=full --show-leak-kinds=all ./main 16 block image1.pgm image1_neg.pgm
	#valgrind -s --leak-check=full --show-leak-kinds=all ./main 16 numbers image2.pgm image2_neg.pgm
	#valgrind -s --leak-check=full --show-leak-kinds=all ./main 16 block image2.pgm image2_neg.pgm


clean:
	rm -f main
