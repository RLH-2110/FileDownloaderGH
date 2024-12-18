#include "downloader.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>

#ifndef NULL
#define NULL (void*)0
#endif


/* strcmp, but return 0xff if one of the strings is a NULL pointer*/
int strcmpp(char* a, char* b) {
	if (a == 0 || b == 0)
		return 0xff;
	else return strcmp(a, b);
}

int main(){

	/* test 1 */
	if (strcmpp(parse_URL("https://www.github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt").hostname,"www.github.com") != 0 ){
		puts("url test 1 failed");
		return 1;
	}
	else { puts("url test 1 passed"); }

	/* test 2 */
	if (strcmpp(parse_URL("https://github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt").hostname, "www.github.com") != 0 ){
		puts("url test 2 failed");
		return 1;
	}
	else { puts("url test 2 passed"); }

	/* test 3*/
	if (strcmpp(parse_URL("www.github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt").hostname, "www.github.com") != 0 ){
		puts("url test 3 failed");
		return 1;
	}
	else { puts("url test 3 passed"); }

	/* test 4 */
	if (strcmpp(parse_URL("github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt").hostname,"www.github.com") != 0 ){
		puts("url test 4 failed");
		return 1;
	}
	else { puts("url test 4 passed"); }

	/* test 5 */
	if (strcmpp(parse_URL("https://www.github.com").hostname, "www.github.com") != 0 ){
		puts("url test 5 failed");
		return 1;
	}
	else { puts("url test 5 passed"); }

	/* test 6 */
	if (strcmpp(parse_URL("https://github.com").hostname, "www.github.com") != 0 ){
		puts("url test 6 failed");
		return 1;
	}
	else { puts("url test 6 passed"); }

	/* test 7 */
	if (strcmpp(parse_URL("www.github.com").hostname,"www.github.com") != 0 ){
		puts("url test 7 failed");
		return 1;
	}
	else { puts("url test 7 passed"); }

	/* test 8 */
	if (strcmpp(parse_URL("github.com").hostname, "www.github.com") != 0 ){
		puts("url test 8 failed");
		return 1;
	}
	else { puts("url test 8 passed"); }

	/* test invalid urls*/

	/* test 9 */
	if (parse_URL(NULL).hostname != NULL ){
		puts("url test 9 failed");
		return 1;
	}
	else { puts("url test 9 passed"); }

	/* test 10 */
	if (parse_URL("()()").hostname != NULL ){
		puts("url test 10 failed");
		return 1;
	}
	else { puts("url test 10 passed"); }

	/* test 11 */
	if (parse_URL("..").hostname != NULL ){
		puts("url test 11 failed");
		return 1;
	}
	else { puts("url test 11 passed"); }

	/* test 12 */
	if (parse_URL(".").hostname != NULL ){
		puts("url test 12 failed");
		return 1;
	}
	else { puts("url test 12 passed"); }

	/* test 13 */
	if (parse_URL("abn../").hostname != NULL ){
		puts("url test 13 failed");
		return 1;
	}
	else { puts("url test 13 passed"); }

	/* test 14 */
	if (parse_URL("https://abc.deg.hij..lhagas.asgdsag/test").hostname != NULL) {
		puts("url test 14 failed");
		return 1;
	}
	else { puts("url test 14 passed"); }

	// valid multiple subdomains

	/* test 15 */
	if (strcmpp(parse_URL("abc.deg.hij.lhagas.asgdsag").hostname, "abc.deg.hij.lhagas.asgdsag") != 0) {
		puts("url test 15 failed");
		return 1;
	}
	else { puts("url test 15 passed"); }

	/* test 16 */
	if (strcmpp(parse_URL("https://abc.deg.hij.lhagas.asgdsag/test").hostname, "abc.deg.hij.lhagas.asgdsag") != 0) {
		puts("url test 16 failed");
		return 1;
	}
	else { puts("url test 16 passed"); }

	/* test 17 */
	if (strcmpp(parse_URL("abc.deg.hij.lhagas.asgdsag/test").hostname, "abc.deg.hij.lhagas.asgdsag") != 0) {
		puts("url test 17 failed");
		return 1;
	}
	else { puts("url test 17 passed"); }

	/* test 18 */
	if (strcmpp(parse_URL("https://abc.deg.hij.lhagas.asgdsag").hostname, "abc.deg.hij.lhagas.asgdsag") != 0) {
		puts("url test 18 failed");
		return 1;
	}
	else { puts("url test 18 passed"); }

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