# FileDownloaderGH
This is a library able to download files from a URL of raw file content, like raw.githubusercontent.com links.  
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

if you have make installed, just do `make` and the output is in downloader.a with optimizations and no debug symbols. You can also run `make release` for the same result.  
Compiling with debug symbols is also possible, simply run `make debug`.  
  
for testing, you can run `make test` or `make debugtest`, the first is without debugging symbols, the second one is with them.  
  
With `make mini` you can get a small example program called urlcat; just give any URL to a URL leading to a raw text file while using https, and it will print the file to the console.  
  
`make clean` and `make clear` can be used to remove the output files.

> [!NOTE]
> all make targets expect **release** and **mini** will compile with debug symbols and without optimisations

### Testing

Running test.o will show you if all the tests are passed. You can also do `make run` to run test.o automatically.  
The tests include some tests with user input for validation, these are tedious and can be skipped by adding any argument to test.o. alternatively run `make skip`  
  
#### Windows

Go to www.msys2.org and download mysys2 and run the installer.  
  
run these commands:
```
pacman -Syu
pacman -S make
pacman -S gcc
pacman -S git
git clone https://github.com/RLH-2110/FileDownloaderGH.git
cd FileDownloaderGH/
nano Makefile
```
adjust these variables:  
windowsSLLIncludePath  
windowsSLLLibs  

then use Ctrl+s to save and Ctrl+x to exit.  
  
then just run `make`, you can use the Linux instructions for more infos.  

## Usage

### Compile Instructions
In your c project, include downloader.h and tell your compiler to include downloader.a in your source and object files.  
Example with the urlcat example program in this repo:  

#### Linux
```bash
  gcc tests/urlcat/main.c downloader.a -o urlcat.o  -lssl -lcrypto
````

#### Windows 
```bash
  gcc tests/urlcat/main.c downloader.a -o urlcat.o  -L"C:/Program Files/OpenSSL-Win64/lib/VC/x64/MT" -I"C:/Program Files/OpenSSL-Win64/include" -lssl -lcrypto -lws2_32
````
the -L instructions should link to your OpenSSL installation

### Usage in C

In your c project, include downloader.h  
Before your first call to `download_file()` you will call `downloader_init()` and after your last call to `download_file()` you will call `donwloader_cleanup()`.  
  
You will need to create a null terminated list of DNS servers, you can do so like this:  
```c
uint32_t DNS_LIST[] = {
	0x08080808, /* 8.8.8.8 google dns*/
	0x08080404, /* 8.8.4.4 google dns*/
	0 /* null termination */
};
```

You can then download files into RAM like this:

```c
	unsigned char* content = NULL;
	uint32_t filesize = 0;
	downloader_init();
	content = download_file("any url that links to raw content", DNS_LIST, &filesize, NULL);
	donwloader_cleanup();
 ```

Arguments:  


| Name         | Type      | Description                                                                                |
| ------------ | --------- | ------------------------------------------------------------------------------------------ |
| URL          | char\*    | String of the URL where files will be downloaded from.                                     |
| DNS_LIST     | int32_t\* | Null terminated list of 32-bit integer that save the IPv4 of DNS Servers                   |
| out_fileSize | int32_t\* | pointer to an 32-bit integer where the size in bytes of the downloaded file will be stored |
| log          | FILE\*    | Can be set to NULL, if not NULL, the function will write logging text into the stream      |

> [!NOTE]
> The url will be valided in the function, so it is safe to pass in invalid urls.

errno will be set to 0 when no errors occurred, Here are possible errno values:
* EINVAL: invalid parameters
* EIO: SSL error, socket error, WSA startup error or error while reading the https header
* EMOMEM: out of memory
* EHOSTUNREACH: URL could not be resolved with any DNS that was provided

> [!NOTE]
> Empty files will return with NULL, with errno set to 0.

> [!NOTE]
> for testing you can use this known good url: https://raw.githubusercontent.com/RLH-2110/FileDownloaderGH/refs/heads/master/sample.txt

## urlcat

For quick testing of the library, a basic test program was created.  
If you compile with `make mini`, you will get urlcat.o  

urlcat takes one parameter, which is the URL where you want to download content from, for example: https://raw.githubusercontent.com/RLH-2110/FileDownloaderGH/refs/heads/master/sample.txt  
with this, you can quickly test if your URL can be downloaded with this tool.

## Q&A

### Why do it manually and not use another existing library? 

> Because where is the fun in that?
