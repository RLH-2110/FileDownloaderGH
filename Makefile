gcc = gcc -ansi -pedantic -Wall
includes = 
files = main.c
output = downloader.lib
cleanup = test.o log.txt

# OS FLAGS
ifeq ($(OS),Windows_NT)
    OSFLAG = -D WINDOWS
$(info    detected windows)
else
	OSFLAG = -D POSIX
$(info    assuming posix)
endif

all:
	$(gcc) -g $(files) $(includes) -c -o $(output) $(OSFLAG)
	$(gcc) -g test.c $(output) -o test.o
	

PHONEY: clear clean run

run: all
	./test.o

clear: clean
clean:
	rm -f $(output)
	rm -f $(cleanup)
