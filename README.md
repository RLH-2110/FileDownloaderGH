# FileDownloaderGH
This is a library able to download files from a URL.  
It has only been tested with files hosted on GitHub, and the project is not entirely done.  
It's usable right now, but I advise against using it at its current state.
  
## Dependencies

You need to have a 64 bit version of OpenSSL installed.

#### Linux

do `sudo apt-get install libssl-dev` and you are done

#### Windows

compile OpenSSH from source or download it from here: https://slproweb.com/products/Win32OpenSSL.html  
after you have downloaded it, you might want to go to the makefile and adjust the paths for `windowsSLLIncludePath` and `windowsSLLLibs`

## Compiling

If you don't have make installed, install it with `sudo apt install make`  

if you have make installed, just do `make` and the output is in test.o and downloader.a with optimizations and no debug symbols. You can also run `make release` for the same result.  
Compiling with debug symbols is also possible, simply run `make all`.  

With `make mini` you can get a small example program called urlcat; just give any URL to a URL leading to a raw text file while using https, and it will print the file to the console.  

> [!NOTE]
> all make targets expect **release** and **mini** will compile with debug symbols and without optimisations

### Testing

Running test.o will show you if all the tests are passed. You can also do `make test` to run test.o automatically.  
The tests include some tests with user input for validation, these are tedious and can be skipped by adding any argument to test.o. alternatively run `make skip`  
to clear the output files, run `make clean`  
  
#### Windows

you can either use the Linux subsystem for Windows or MinGW, I will add links to both later.

## Q&A

### Why do it manually and not use a library? 

> Because where is the fun in that?
