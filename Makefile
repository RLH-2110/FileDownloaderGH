gcc = gcc -ansi -pedantic -Wall
files = main_*.c helpers.c
ofiles := $(files:.c=.o)
output = downloader.a
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
	$(gcc) -g $(files) -c $(OSFLAG)

	ar cr $(output) $(ofiles)
	$(gcc) -g test.c $(output) -o test.o
	

PHONEY: clear clean run

run: all
	./test.o

clear: clean
clean:
	rm -f $(output)
	rm -f $(cleanup)
	rm -f $(ofiles)
