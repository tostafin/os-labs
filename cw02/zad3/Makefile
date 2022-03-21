CC = gcc
CFLAGS = -std=c11 -Wall

all: test

test: clean test_dir test_nftw

test_dir:
	@printf "USING DIR FUNCTIONS:\n"
	@$(CC) $(CFLAGS) main.c -o main
	@./main ./directory

test_nftw:
	@printf "USING NFTW:\n"
	@$(CC) $(CFLAGS) main.c -o main -D USE_NFTW
	@./main ./directory

clean:
	@rm -f main
