CC = gcc
CFLAGS = -Wall
OBJS = main.o cook.o supplier.o common.o

all: compile clean

compile: $(OBJS)
	$(CC) $(CFLAGS) main.o common.o -o main -lm
	$(CC) $(CFLAGS) cook.o common.o -o cook -lm
	$(CC) $(CFLAGS) supplier.o common.o -o supplier -lm

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

cook.o: cook.c
	$(CC) $(CFLAGS) -c cook.c

supplier.o: supplier.c
	$(CC) $(CFLAGS) -c supplier.c

common.o: common.c
	$(CC) $(CFLAGS) -c common.c

clean:
	@rm -rf $(OBJS)

cle:
	@rm -rf main cook supplier

clmem:
	@ipcrm --all=shm
	@ipcrm --all=sem