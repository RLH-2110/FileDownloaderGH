#include "downloader.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>

#ifndef NULL
#define NULL (void*)0
#endif


/* zero if equal, non zero if not equal.*/
int compare(char* a, char* b) {
	if (a == 0 && b == 0)
		return 0;
	if (a == 0 || b == 0)
		return 0xff;
	return strcmp(a, b);
}

int main(){

	#define numTests 20
	char* testUrls[numTests] = {
		"https://www.github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt", /* 00 */
		"https://github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
		"www.github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
		"github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
		"https://www.github.com", 
		"https://github.com", /* 05 */
		"www.github.com",
		"github.com",
		NULL,
		"()()",
		"..", /* 10 */
		".",
		"abn.. / ",
		"https://abc.deg.hij..lhagas.asgdsag/test",
		"abc.deg.hij.lhagas.asgdsag",
		"https://abc.deg.hij.lhagas.asgdsag/test", /* 15 */
		"abc.deg.hij.lhagas.asgdsag/test",
		"https://abc.deg.hij.lhagas.asgdsag",
		"http://www.github.com",
		"ftp://www.github.com"
	};

	char* expectedHostname[numTests] = {
		"www.github.com", /* 00 */
		"www.github.com",
		"www.github.com",
		"www.github.com",
		"www.github.com",
		"www.github.com", /* 05 */
		"www.github.com",
		"www.github.com",
		NULL,
		NULL,
		NULL, /* 10 */
		NULL,
		NULL,
		NULL,
		"abc.deg.hij.lhagas.asgdsag",
		"abc.deg.hij.lhagas.asgdsag", /* 15 */
		"abc.deg.hij.lhagas.asgdsag",
		"abc.deg.hij.lhagas.asgdsag",
		"www.github.com",
		"www.github.com"
	};

	char* expectedProtocol[numTests] = {
		"https", /* 00 */
		"https",
		"https",
		"https",
		"https",
		"https", /* 05 */
		"https",
		"https",
		NULL,
		NULL,
		NULL, /* 10 */
		NULL,
		NULL,
		NULL,
		"https",
		"https", /* 15 */
		"https",
		"https",
		"http",
		"ftp"
	};

	char* expectedRest[numTests] = {
		"/RLH-2110/FileDownloaderGH/blob/master/sample.txt", /* 00 */
		"/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
		"/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
		"/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
		"",
		"", /* 05 */
		"",
		"",
		NULL,
		NULL,
		NULL, /* 10 */
		NULL,
		NULL,
		NULL,
		"",
		"/test", /* 15 */
		"/test",
		"",
		"",
		""
	};

	parsedUrl result;

	for (int i = 0; i < numTests;i++) {
		result = parse_URL(testUrls[i]);

		if (compare(result.hostname, expectedHostname[i]) != 0) {
			printf("url test 1A failed\n");
			return 1;
		}
		else { printf("url test %dA passed\n",i); }

		/* test 1B */
		if (compare(result.protocol, expectedProtocol[i]) != 0) {
			printf("url test %dB failed\n",i);
			return 1;
		}
		else { printf("url test %dB passed\n",i); }

		/* test 1C */
		if (compare(result.rest, expectedRest[i]) != 0) {
			printf("url test %dC failed\n",i);
			return 1;
		}
		else { printf("url test %dC passed\n",i); }

		free(result.protocol);
		free(result.hostname);
		free(result.rest);

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