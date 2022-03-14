CC = gcc
CFLAGS = -std=c11 -Wall -Werror -g

all: test clean

static:
	@$(CC) $(CFLAGS) -c library.c
	@ar cr liblibrary.a library.o
	@$(CC) main.c -o main -L. -llibrary

test: static cleanReport test_1 test_2 test_3

test_1:
	@printf "***TEST 1***\n" >> raport2.txt
	@printf "1 maly plik, liczony 10 razy, za każdym razem o jeden plik więcej na raz, tablica o rozmiarze 50, calosc powtorzona 3 razy\n\n" >> raport2.txt
	@./main `cat ./files/command1.txt`
	@printf "***KONIEC TESTU 1***\n\n" >> raport2.txt

test_2:
	@printf "***TEST 2***\n" >> raport2.txt
	@printf "1 maly plik, 1 sredni, 1 duzy, liczone za kazdym razem o jeden wiecej, tablica o rozmiarze 5000, calosc powtorzona 3 razy\n\n" >> raport2.txt
	@./main `cat ./files/command2.txt`
	@printf "***KONIEC TESTU 2***\n\n" >> raport2.txt

test_3:
	@printf "***TEST 3***\n" >> raport2.txt
	@printf "5 malych plikow, 3 srednie, 2 duze, liczone za kazdym razem o jeden raz wiecej, tablica o rozmiarze 50000, calosc powtorzona 3 razy\n\n"
	@./main `cat ./files/command3.txt`
	@printf "***KONIEC TESTU 3***\n\n" >> raport2.txt

clean:
	@rm -f *.o *.a

cleanReport:
	@rm -f raport2.txt