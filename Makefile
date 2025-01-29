gcc = gcc -ansi -pedantic -Wall -D BITS64
files_stage1 = main_*.c helpers.c urlParse/urlParse.c
files_stage2 = tests/test.c tests/getc.c tests/ipinput.c tests/testing_funcs.c urlParse/urlParse_test.c urlParse/urlParse_testData.c
#ofiles := $(files_stage1:.c=.o)
ofiles := $(patsubst %.c,%.o,$(notdir $(files_stage1)))
output = downloader.a
cleanup = test.o log.txt
ldflags =
warnings = -Wno-long-long

# for windows only
windowsSLLIncludePath = "C:/Program Files/OpenSSL-Win64/include"
windowsSLLLibs = "C:/Program Files/OpenSSL-Win64/lib/VC/x64/MT"

# OS FLAGS
ifeq ($(OS),Windows_NT)
    OSFLAG = -D WINDOWS
	LDFLAGS = -lws2_32 -L$(windowsSLLLibs) -I$(windowsSLLIncludePath) -lssl -lcrypto
$(info    detected windows)
else
	OSFLAG = -D POSIX
	LDFLAGS = -lssl -lcrypto 
$(info    assuming posix)
endif

all:
	$(gcc) -g $(files_stage1) -c $(LDFLAGS) $(OSFLAG) $(warnings)

	ar cr $(output) $(ofiles)
	$(gcc) -g $(files_stage2) $(output) -o test.o $(LDFLAGS) $(OSFLAG) $(warnings)
	

PHONY: clear clean run skip

run: all
	./test.o

skip: all
	./test.o skip

clear: clean
clean:
	rm -f $(output)
	rm -f $(cleanup)
	rm -f $(ofiles)
	rm -f ./resp.txt
	rm -f ./log.txt
	
