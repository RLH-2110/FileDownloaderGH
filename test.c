#include "downloader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

/* gets a sting, if its NULl it retuns "NULL", if its empty it returns "(EMPTY)" if its neither, it returns the given string*/
char* got(char* str){
	if (str == NULL)
		return "NULL";
	if (str[0] == 0)
		return "(EMPTY)";
	return str;
}

int main(){

	#define num_parse_URL_Tests 20
	char* testUrls[num_parse_URL_Tests] = {
		"https://www.github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt", /* 01 */
		"https://github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
		"www.github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
		"github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
		"https://www.github.com",  /* 05 */
		"https://github.com",
		"www.github.com",
		"github.com",
		NULL,
		"()()", /* 10 */
		"..", 
		".",
		"abn.. / ",
		"https://abc.deg.hij..lhagas.asgdsag/test",
		"abc.deg.hij.lhagas.asgdsag", /* 15 */
		"https://abc.deg.hij.lhagas.asgdsag/test", 
		"abc.deg.hij.lhagas.asgdsag/test",
		"https://abc.deg.hij.lhagas.asgdsag",
		"http://www.github.com",
		"ftp://www.github.com" /* 20 */
	};

	char* expectedHostname[num_parse_URL_Tests] = {
		"www.github.com", /* 01 */
		"www.github.com",
		"www.github.com",
		"www.github.com",
		"www.github.com",/* 05 */
		"www.github.com", 
		"www.github.com",
		"www.github.com",
		NULL,
		NULL,/* 10 */
		NULL, 
		NULL,
		NULL,
		NULL,
		"abc.deg.hij.lhagas.asgdsag",/* 15 */
		"abc.deg.hij.lhagas.asgdsag", 
		"abc.deg.hij.lhagas.asgdsag",
		"abc.deg.hij.lhagas.asgdsag",
		"www.github.com",
		"www.github.com" /* 20 */
	};

	char* expectedProtocol[num_parse_URL_Tests] = {
		"https", /* 01 */
		"https",
		"https",
		"https",
		"https",  /* 05 */
		"https",
		"https",
		"https",
		NULL,
		NULL,/* 10 */
		NULL, 
		NULL,
		NULL,
		NULL,
		"https", /* 15 */
		"https", 
		"https",
		"https",
		"http",
		"ftp" /* 20 */
	};

	char* expectedRest[num_parse_URL_Tests] = {
		"/RLH-2110/FileDownloaderGH/blob/master/sample.txt", /* 01 */
		"/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
		"/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
		"/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
		"", /* 05 */
		"", 
		"",
		"",
		NULL,
		NULL,  /* 10 */
		NULL,
		NULL,
		NULL,
		NULL,
		"", /* 15 */
		"/test", 
		"/test",
		"",
		"",
		"" /* 20 */
	};


	#define num_QNAME_Tests 5

	char* testHostnames[num_QNAME_Tests] = {
		"www.github.com",
		"github",
		"",
		"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
		"ABC.DEF.GHI.JKL.asg-gasdg.sd.d.hsdhsdh.g"
	};

	char* expectedQNAME[num_QNAME_Tests] = {
		"3www6github3com0",
		"6github0",
		"0",
		NULL,
		"3ABC3DEF3GHI3JKL9asg-gasdg2sd1d7hsdhsdh1g"
	};



	parsedUrl result;
	char* sresult;
	int i;

	for (i = 0; i < num_parse_URL_Tests;i++) {
		result = parse_URL(testUrls[i]);

		if (compare(result.hostname, expectedHostname[i]) != 0) {
			printf("parse_URL test %dA (hostname) failed\nexpected: %s\ngot: %s\n",i+1,got(expectedHostname[i]),got(result.hostname));
			printf("test passed (%d out of %d)\n", i + 1, num_parse_URL_Tests);
			return 1;
		}
		else { printf("parse_URL test %dA passed\n",i+1); }

		/* test 1B */
		if (compare(result.protocol, expectedProtocol[i]) != 0) {
			printf("parse_URL test %dB (protocol) failed\nexpected: %s\ngot: %s\n",i+1,got(expectedProtocol[i]),got(result.protocol));
			printf("test passed (%d out of %d)\n", i + 1, num_parse_URL_Tests);
			return 1;
		}
		else { printf("parse_URL test %dB passed\n",i+1); }

		/* test 1C */
		if (compare(result.rest, expectedRest[i]) != 0) {
			printf("parse_URL test %dC (rest) failed\nexpected: %s\ngot: %s\n",i+1,got(expectedRest[i]),got(result.rest));
			printf("test passed (%d out of %d)\n", i+1, num_parse_URL_Tests);
			return 1;
		}
		else { printf("parse_URL test %dC passed\n",i+1); }

		free(result.protocol);
		free(result.hostname);
		free(result.rest);

	}

	printf("parse_URL tests passed (%d out of %d)\n", i,num_parse_URL_Tests);





	/* getQNAME(); */


	for (i = 0; i < num_QNAME_Tests;i++) {
		sresult = getQNAME(testHostnames[i]);

		if (compare(sresult, expectedQNAME[i]) != 0) {
			printf("getQNAME test %d failed\nexpected: %s\ngot: %s\n",i+1,got(expectedQNAME[i]),got(sresult));
			printf("test passed (%d out of %d)\n", i + 1, num_QNAME_Tests);
			return 1;
		}
		else { printf("getQNAME test %d passed\n",i+1); }

		free(sresult);
	}

	printf("getQNAME tests passed (%d out of %d)\n", i,num_QNAME_Tests);




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