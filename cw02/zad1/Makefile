CC = gcc
CFLAGS = -std=c11 -Wall -Werror -g

all: test

test: clean test_lib test_sys

test_lib:
	@printf "Wariant lib:\n" >> pomiar_zad_1.txt
	@$(CC) $(CFLAGS) main.c -o main -D USE_LIB
	@(time ./main file1.txt file2.txt) 2>&1 | tee -a pomiar_zad_1.txt
	@printf "\n\n" >> pomiar_zad_1.txt

test_sys:
	@printf "Wariant sys:\n" >> pomiar_zad_1.txt
	@$(CC) $(CFLAGS) main.c -o main -D USE_SYS
	@(time ./main file1.txt file2.txt) 2>&1 | tee -a pomiar_zad_1.txt

clean:
	@rm -f pomiar_zad_1.txt
