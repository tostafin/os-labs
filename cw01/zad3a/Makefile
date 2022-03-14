CC = gcc
CFLAGS = -std=c11 -Wall
REPORT_NAME = raport3a.txt
LIB_PATH = ../zad1/library.c
MAIN_PATH = ../zad2/main.c

all: test clean

static:
	@$(CC) -c $(LIB_PATH)
	@ar cr liblibrary.a library.o
	@$(CC) $(CFLAGS) $(MAIN_PATH) -o main -L. -llibrary

shared:
	@$(CC) -fPIC -c $(LIB_PATH)
	@$(CC) -shared -fPIC -o liblibrary.so library.o
	@$(CC) $(CFLAGS) $(MAIN_PATH) -o main -L. -llibrary -Wl,-rpath=$(PWD)

dynamic: shared
	@$(CC) $(CFLAGS) $(MAIN_PATH) -o main -ldl -D USE_DYNAMIC_LIBRARY

test: clean_report
	@printf "*****BIBLIOTEKA STATYCZNA*****\n" >> $(REPORT_NAME)
	@make static
	@make run_tests
	@printf "*****BIBLIOTEKA DZIELONA*****\n" >> $(REPORT_NAME)
	@make shared
	@make run_tests
	@printf "*****BIBLIOTEKA LADOWANA DYNAMICZNIE*****\n" >> $(REPORT_NAME)
	@make dynamic
	@make run_tests
	@make clean_libraries

run_tests: test_1 test_2 test_3

test_1:
	@printf "***TEST 1***\n" >> $(REPORT_NAME)
	@printf "1 maly plik, liczony 10 razy, za kazdym razem o jeden plik wiecej na raz, tablica o rozmiarze 50, \
calosc powtorzona 3 razy\n\n" >> $(REPORT_NAME)
	@./main `cat ./commands/command1.txt` $(REPORT_NAME)
	@printf "***KONIEC TESTU 1***\n\n" >> $(REPORT_NAME)

test_2:
	@printf "***TEST 2***\n" >> $(REPORT_NAME)
	@printf "1 maly plik, 1 sredni, 1 duzy, liczone za kazdym razem o jeden wiecej, tablica o rozmiarze 5000, \
	calosc powtorzona 3 razy\n\n" >> $(REPORT_NAME)
	@./main `cat ./commands/command2.txt` $(REPORT_NAME)
	@printf "***KONIEC TESTU 2***\n\n" >> $(REPORT_NAME)

test_3:
	@printf "***TEST 3***\n" >> $(REPORT_NAME)
	@printf "5 malych plikow, 3 srednie, 2 duze, liczone za kazdym razem o jeden raz wiecej, \
tablica o rozmiarze 50000, calosc powtorzona 3 razy\n\n" >> $(REPORT_NAME)
	@./main `cat ./commands/command3.txt` $(REPORT_NAME)
	@printf "***KONIEC TESTU 3***\n\n" >> $(REPORT_NAME)

clean:
	@rm -f *.o *.a main

clean_libraries:
	@rm -f *.so

clean_report:
	@rm -f $(REPORT_NAME)
