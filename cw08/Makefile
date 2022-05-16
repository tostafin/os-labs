CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread

all: compile run_v

compile:
	$(CC) $(CFLAGS) main.c -o main $(LIBS)

run:
	./main 3 numbers image.pgm image_neg.pgm
	#./main 500 numbers baboon.ascii.pgm baboon_neg.ascii.pgm
	#./main 500 numbers apollonian_gasket.ascii.pgm apollonian_gasket_neg.ascii.pgm
	#./main 500 numbers pepper.ascii.pgm pepper_neg.ascii.pgm

run_v:
	valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all ./main 3 numbers image.pgm image_neg.pgm
	#valgrind -s --leak-check=full --show-leak-kinds=all ./main 100 numbers baboon.ascii.pgm baboon_neg.ascii.pgm
	#valgrind -s --leak-check=full --show-leak-kinds=all ./main 51 numbers apollonian_gasket.ascii.pgm apollonian_gasket_neg.ascii.pgm
	#valgrind -s --leak-check=full --show-leak-kinds=all ./main 500 numbers pepper.ascii.pgm pepper_neg.ascii.pgm

clean:
	rm -f main
