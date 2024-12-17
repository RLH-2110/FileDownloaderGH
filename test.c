#include "downloader.h"
#include <stdio.h>
#include <memory.h>

#ifndef NULL
#define NULL (void*)0
#endif


int main(){

	if (download_file("https://github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt") != NULL){
		puts("calling download_file without initalizing the DNS_LIST should return NULL!");
	}

	DNS_LIST = malloc(sizeof(int32) * 3);
	DNS_LIST[0] = 0x08080808; /* 8.8.8.8 google dns*/
	DNS_LIST[1] = 0x08080404; /* 8.8.4.4 google dns*/
	DNS_LIST[2] = 0; /* null termination */
	

	if (download_file(NULL) != NULL){
		puts("passing NULL to download_file should return NULL!");
	}

	if (download_file("()()()") != NULL){
		puts("passing invalid url to download_file should return NULL!");
	}


	puts(download_file("https://github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt"));
	return 0;
}