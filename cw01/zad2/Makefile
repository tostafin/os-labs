CC = gcc
CFLAGS = -std=c11 -Wall
REPORT_FILE = raport2.txt
LIB_PATH = ../zad1/library.c

all: test clean

static:
	@$(CC) -c $(LIB_PATH)
	@ar cr liblibrary.a library.o
	@$(CC) $(CFLAGS) main.c -o main -L. -llibrary

test: static clean_report test_1 test_2 test_3

test_1:
	@printf "***TEST 1***\n" >> $(REPORT_FILE)
	@printf "1 maly plik, liczony 10 razy, za kazdym razem o jeden plik wiecej na raz, tablica o rozmiarze 50, \
calosc powtorzona 3 razy\n\n" >> $(REPORT_FILE)
	@./main `cat ./files/command1.txt` $(REPORT_FILE)
	@printf "***KONIEC TESTU 1***\n\n" >> $(REPORT_FILE)

test_2:
	@printf "***TEST 2***\n" >> $(REPORT_FILE)
	@printf "1 maly plik, 1 sredni, 1 duzy, liczone za kazdym razem o jeden wiecej, tablica o rozmiarze 5000, \
calosc powtorzona 3 razy\n\n" >> $(REPORT_FILE)
	@./main `cat ./files/command2.txt` $(REPORT_FILE)
	@printf "***KONIEC TESTU 2***\n\n" >> $(REPORT_FILE)

test_3:
	@printf "***TEST 3***\n" >> $(REPORT_FILE)
	@printf "5 malych plikow, 3 srednie, 2 duze, liczone za kazdym razem o jeden raz wiecej, \
tablica o rozmiarze 50000, calosc powtorzona 3 razy\n\n" >> $(REPORT_FILE)
	@./main `cat ./files/command3.txt` $(REPORT_FILE)
	@printf "***KONIEC TESTU 3***\n\n" >> $(REPORT_FILE)

clean:
	@rm -f *.o *.a

clean_report:
	@rm -f $(REPORT_FILE)
