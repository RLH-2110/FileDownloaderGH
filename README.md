This *will* be a library able to download files from a URL. 
  
## dependencies

You need to have a 64 bit version of OpenSSL installed.

#### Linux

do `sudo apt-get install libssl-dev` and you are done

#### Windows

compile OpenSSH from source or download it from here: https://slproweb.com/products/Win32OpenSSL.html  
after you have downloaded it, you might want to go to the makefile and adjust the paths for `windowsSLLIncludePath` and `windowsSLLLibs`

## Compiling

if you have make installed, just do `make` and the output is in test.o and downloader.a  
running test.o will show you if all the tests are passed. You can also do `make test` to run test.o automatically.  
The tests include some tests with user input for validation, these are tedious and can be skipped by adding any argument to test.o. alternatively run `make skip`  
to clear the output files, run `make clean`  

if you don't have make installed, try `sudo apt install make`  

#### Windows

you can either use the Linux subsystem for Windows or MinGW, I will add links to both later.

### Why do it manually and not use a library? 

> Because where is the fun in that?
