CC = gcc
CFLAGS = -std=c11 -Wall -Werror -g

all: clean test clean_wn

test:
	@printf "Szerokość: 0.0002, n = 10000\n" | tee -a pomiar_zad_2.txt
	@$(CC) $(CFLAGS) main.c -o main
	@(time ./main 0.00001 10000) 2>&1 | tee -a pomiar_zad_2.txt
	@printf "\n\n" | tee -a pomiar_zad_2.txt
	
	@printf "Szerokość: 0.0001, n = 20000\n" | tee -a pomiar_zad_2.txt
	@$(CC) $(CFLAGS) main.c -o main
	@(time ./main 0.00002 20000) 2>&1 | tee -a pomiar_zad_2.txt
	@printf "\n\n" | tee -a pomiar_zad_2.txt
	
	@printf "Szerokość: 0.00005, n = 30000\n" | tee -a pomiar_zad_2.txt
	@$(CC) $(CFLAGS) main.c -o main
	@(time ./main 0.00003 30000) 2>&1 | tee -a pomiar_zad_2.txt
	@printf "\n" | tee -a pomiar_zad_2.txt


clean:
	@rm -f pomiar_zad_2.txt main w*

clean_wn:
	@rm -f w*
