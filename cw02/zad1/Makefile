CC = gcc
CFLAGS = -std=c11 -Wall

all: test

test: clean test_lib test_sys

test_lib:
	@printf "Wariant lib:" | tee -a pomiar_zad_1.txt
	@$(CC) $(CFLAGS) main.c -o main -D USE_LIB
	@(time ./main file1.txt file2.txt) 2>&1 | tee -a pomiar_zad_1.txt
	@printf "\n\n" | tee -a pomiar_zad_1.txt

test_sys:
	@printf "Wariant sys:" | tee -a pomiar_zad_1.txt
	@$(CC) $(CFLAGS) main.c -o main -D USE_SYS
	@(time ./main file1.txt file2.txt) 2>&1 | tee -a pomiar_zad_1.txt

clean:
	@rm -f pomiar_zad_1.txt main
