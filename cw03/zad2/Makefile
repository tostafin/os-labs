CC = gcc
CFLAGS = -Wall

all: clean test clean_after

test:
	@printf "Szerokosc: 0.0005, n = 2000\n" | tee -a pomiar_zad_2.txt
	@$(CC) $(CFLAGS) main.c -o main
	@(time ./main 0.0005 2000) 2>&1 | tee -a pomiar_zad_2.txt
	@printf "\n\n" | tee -a pomiar_zad_2.txt
	
	@printf "Szerokosc: 0.00025, n = 4000\n" | tee -a pomiar_zad_2.txt
	@$(CC) $(CFLAGS) main.c -o main
	@(time ./main 0.00025 4000) 2>&1 | tee -a pomiar_zad_2.txt
	@printf "\n\n" | tee -a pomiar_zad_2.txt
	
	@printf "Szerokosc: 0.0002, n = 5000\n" | tee -a pomiar_zad_2.txt
	@$(CC) $(CFLAGS) main.c -o main
	@(time ./main 0.0002 5000) 2>&1 | tee -a pomiar_zad_2.txt
	@printf "\n" | tee -a pomiar_zad_2.txt


clean:
	@rm -f w* main pomiar_zad_2.txt

clean_after:
	@rm -f w* main

