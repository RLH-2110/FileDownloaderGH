gcc = gcc -ansi -pedantic -Wall
includes = 
files = main.c
output = downloader.lib

all:
	$(gcc) $(files) $(includes) -c -o $(output)
	$(gcc) test.c $(output) -o test.o
	

PHONEY: clear clean run

run: all
	./test.o

clear: clean
clean:
	rm -f $(output)