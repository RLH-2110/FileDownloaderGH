This *will* be a library able to download files from a URL. 
  
## dependencies ##

You need to have OpenSSL installed.

#### Linux

do `sudo apt-get install libssl-dev` and you are done

#### Windows

compile OpenSSH from source or download it from here: https://slproweb.com/products/Win32OpenSSL.html  
after you have downloaded it, you might want to go to the makefile and adjust the paths for `windowsSLLIncludePath` and `windowsSLLLibs`

### Why do it manually and not use a library? 

> Because where is the fun in that?
