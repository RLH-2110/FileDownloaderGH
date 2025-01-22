gcc = gcc -ansi -pedantic -Wall
files_stage1 = main_*.c helpers.c
files_stage2 = test.c getc.c ipinput.c
ofiles := $(files_stage1:.c=.o)
output = downloader.a
cleanup = test.o log.txt
ldflags =

# OS FLAGS
ifeq ($(OS),Windows_NT)
    OSFLAG = -D WINDOWS
	LDFLAGS = -lws2_32 -lsecur32.lib
$(info    detected windows)
else
	OSFLAG = -D POSIX
	LDFLAGS = -lssl -lcrypto
$(info    assuming posix)
endif

all:
	$(gcc) -g $(files_stage1) -c $(OSFLAG)

	ar cr $(output) $(ofiles)
	$(gcc) -g $(files_stage2) $(output) -o test.o $(LDFLAGS) $(OSFLAG)
	

PHONY: clear clean run

run: all
	./test.o

clear: clean
clean:
	rm -f $(output)
	rm -f $(cleanup)
	rm -f $(ofiles)
