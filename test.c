#include "downloader.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>

#ifndef NULL
#define NULL (void*)0
#endif


int main(){


	/* test 1 */
	if (strcmp(trim_URL("https://www.github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt"),"www.github.com") != 0 ){
		puts("url test 1 failed");
		return 1;
	}

	/* test 2 */
	if (strcmp(trim_URL("https://github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt"), "www.github.com") != 0 ){
		puts("url test 2 failed");
		return 1;
	}

	/* test 3*/
	if (strcmp(trim_URL("www.github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt"), "www.github.com") != 0 ){
		puts("url test 3 failed");
		return 1;
	}

	/* test 4 */
	if (strcmp(trim_URL("github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt"),"www.github.com") != 0 ){
		puts("url test 4 failed");
		return 1;
	}

	/* test 5 */
	if (strcmp(trim_URL("https://www.github.com"), "www.github.com") != 0 ){
		puts("url test 5 failed");
		return 1;
	}

	/* test 6 */
	if (strcmp(trim_URL("https://github.com"), "www.github.com") != 0 ){
		puts("url test 6 failed");
		return 1;
	}

	/* test 7 */
	if (strcmp(trim_URL("www.github.com"),"www.github.com") != 0 ){
		puts("url test 7 failed");
		return 1;
	}

	/* test 8 */
	if (strcmp(trim_URL("github.com"), "www.github.com") != 0 ){
		puts("url test 8 failed");
		return 1;
	}

	/* test 9 */
	if (trim_URL(NULL) != NULL ){
		puts("url test 9 failed");
		return 1;
	}

	/* test 10 */
	if (trim_URL("()()") != NULL ){
		puts("url test 10 failed");
		return 1;
	}

	/* test 11 */
	if (trim_URL("..") != NULL ){
		puts("url test 11 failed");
		return 1;
	}

	/* test 12 */
	if (trim_URL(".") != NULL ){
		puts("url test 12 failed");
		return 1;
	}

	/* test 13 */
	if (trim_URL("abn../") != NULL ){
		puts("url test 13 failed");
		return 1;
	}

	/* test 14 */
	if (trim_URL("abc.deg.hij.lhagas.asgdsag") != NULL ){
		puts("url test 14 failed");
		return 1;
	}

	/*if (download_file("https://github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt") != NULL){
		puts("calling download_file without initalizing the DNS_LIST should return NULL!");
		return 1;
	}

	DNS_LIST = malloc(sizeof(int32) * 3);
	DNS_LIST[0] = 0x08080808; /* 8.8.8.8 google dns*/
/*	DNS_LIST[1] = 0x08080404; /* 8.8.4.4 google dns*/
/*	DNS_LIST[2] = 0; /* null termination */
/*	

	if (download_file(NULL) != NULL){
		puts("passing NULL to download_file should return NULL!");
		return 1;
	}

	if (download_file("()()()") != NULL){
		puts("passing invalid url to download_file should return NULL!");
		return 1;
	}


	puts(download_file("https://github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt"));/**/
	return 0;
} 