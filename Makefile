gcc = gcc -ansi -pedantic -Wall
files_test_stage = tst.c tests/getc.c tests/ipinput.c
#ofiles := $(files_stage1:.c=.o)
ofiles := $(patsubst %.c,%.o,$(notdir $(files_stage1)))
output = downloader.a
cleanup = test.o log.txt urlcat.o
warnings = -Wno-long-long

# for windows only
windowsSLLIncludePath = "C:/Program Files/OpenSSL-Win64/include"
windowsSLLLibs = "C:/Program Files/OpenSSL-Win64/lib/VC/x64/MT"

# OS FLAGS
ifeq ($(OS),Windows_NT)
    OSFLAG = -D WINDOWS
	LDFLAGS += -lws2_32 -L$(windowsSLLLibs) -I$(windowsSLLIncludePath) -lssl -lcrypto
$(info    detected windows)
else
	OSFLAG = -D POSIX
	LDFLAGS += -lssl -lcrypto 
$(info    assuming posix)
endif

.PHONY: clear clean run skip mini release test debug debugtest windll_relase windll_debug bitbake_release bitbake_mini


release:
	$(gcc) -O3 lib.c -c $(LDFLAGS) $(OSFLAG) -o $(output)

test: release
	$(gcc) -O3 $(files_test_stage) -o test.o $(LDFLAGS) $(OSFLAG) 

debug:
	$(gcc) -g lib.c -c $(LDFLAGS) $(OSFLAG) $(warnings) -o $(output) -D DEBUG

debugtest: debug
	$(gcc) -g $(files_test_stage) -o test.o $(LDFLAGS) $(OSFLAG) $(warnings) -D DEBUG

mini: release
	$(gcc) -O3 tests/urlcat/main.c $(output) -o urlcat.o $(LDFLAGS) $(OSFLAG)

bitbake_release:
	$(CC) -O3 lib.c -c $(LDFLAGS) $(CFLAGS) $(OSFLAG) -o $(output)

bitbake_mini: bitbake_release
	$(CC) -O3 tests/urlcat/main.c $(output) -o urlcat $(LDFLAGS) $(CFLAGS) $(OSFLAG)

run: debugtest
	./test.o

skip: debugtest
	./test.o skip


windll_release: release
	$(gcc) -O3 WINDLL-wrapper.c $(output) -o downloader.dll -shared $(LDFLAGS) -D WINDOWS
windll_debug: debug
	$(gcc) -g WINDLL-wrapper.c $(output) -o downloader.dll $(warnings) -shared $(LDFLAGS) -D WINDOWS

clear: clean
clean:
	rm -f $(output)
	rm -f $(cleanup)
	rm -f $(ofiles)
	rm -f ./resp.txt
	rm -f ./log.txt
	rm -f downloader.dll
	
