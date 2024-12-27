#include "downloader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "testdata.h"

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
	parsedUrl result;
	char* sresult;
	int i;
	int nresult;
	char* str1;
	char* str2;
	int size;
	int32* DNS_LIST;

	for (i = 0; i < num_parse_URL_Tests;i++) {
		result = parse_URL(testUrls[i]);

		if (compare(result.hostname, expectedHostname[i]) != 0) {
			printf("parse_URL test %dA (hostname) failed\nexpected: %s\ngot: %s\n",i+1,got(expectedHostname[i]),got(result.hostname));
			printf("test passed (%d out of %d)\n", i + 1, num_parse_URL_Tests);
			return EXIT_FAILURE;
		}
		else { printf("parse_URL test %dA passed\n",i+1); }

		/* test 1B */
		if (compare(result.protocol, expectedProtocol[i]) != 0) {
			printf("parse_URL test %dB (protocol) failed\nexpected: %s\ngot: %s\n",i+1,got(expectedProtocol[i]),got(result.protocol));
			printf("test passed (%d out of %d)\n", i + 1, num_parse_URL_Tests);
			return EXIT_FAILURE;
		}
		else { printf("parse_URL test %dB passed\n",i+1); }

		/* test 1C */
		if (compare(result.rest, expectedRest[i]) != 0) {
			printf("parse_URL test %dC (rest) failed\nexpected: %s\ngot: %s\n",i+1,got(expectedRest[i]),got(result.rest));
			printf("test passed (%d out of %d)\n", i+1, num_parse_URL_Tests);
			return EXIT_FAILURE;
		}
		else { printf("parse_URL test %dC passed\n",i+1); }

		free(result.protocol);	result.protocol = NULL;
		free(result.hostname);	result.hostname = NULL;
		free(result.rest);		result.rest = NULL;

	}

	printf("parse_URL tests passed (%d out of %d)\n", i,num_parse_URL_Tests);




	/* getQNAME(); */
	for (i = 0; i < num_QNAME_Tests;i++) {
		sresult = getQNAME(testHostnames[i]);

		if (compare(sresult, expectedQNAME[i]) != 0) {
			str1 = debug_get_printable_qname(expectedQNAME[i]);
			str2 = debug_get_printable_qname(sresult);
			printf("getQNAME test %d failed\nexpected: %s\ngot: %s\n",i+1, str1,str2);
			printf("test passed (%d out of %d)\n", i + 1, num_QNAME_Tests);
			free(str1);	free(str2); str1 = NULL; str2 = NULL;
			return EXIT_FAILURE;
		}
		else { printf("getQNAME test %d passed\n",i+1); }

		free(sresult); sresult = NULL;
	}

	printf("getQNAME tests passed (%d out of %d)\n", i,num_QNAME_Tests);



	/* compare_DNS_requests(); */
	for (i = 0; i < num_DNSCMP_Tests;i++) {
		nresult = compare_DNS_requests(test_DNScmpA[i], test_DNScmpB[i]);

		if (excpetedDNScmp[i] == false && nresult != false) {
			printf("compare_DNS_requests test %d failed\nexpected: 0\ngot: %d\n", i + 1, nresult);
			printf("test passed (%d out of %d)\n", i + 1, num_DNSCMP_Tests);
			return EXIT_FAILURE;
		}
		else if (excpetedDNScmp[i] == true && nresult == false) {
			printf("compare_DNS_requests test %d failed\nexpected: not 0\ngot: %d\n", i + 1, nresult);
			printf("test passed (%d out of %d)\n", i + 1, num_DNSCMP_Tests);
			return EXIT_FAILURE;
		} else{
			printf("compare_DNS_requests test %d passed\n", i + 1); 
		}
	}

	printf("compare_DNS_requests tests passed (%d out of %d)\n", i, num_DNSCMP_Tests);

	

	/* debug_get_printable_DNS_request(); */
	for (i = 0; i < num_DNSREQPRINT_Tests;i++) {
		sresult = debug_get_printable_DNS_request(test_DNSRequestPrints[i]);

		if (compare(sresult, expectedDNSRequestPrints[i]) != 0) {
			printf("debug_get_printable_DNS_request test %d failed\nexpected:\n%s\n\ngot:\n%s\n", i + 1, expectedDNSRequestPrints[i], sresult);
			printf("test passed (%d out of %d)\n", i + 1, num_DNSREQPRINT_Tests);
			return EXIT_FAILURE;
		}
		else {
			printf("debug_get_printable_DNS_request test %d passed\n", i + 1);
		}
		free(sresult); sresult = NULL;
	}

	printf("debug_get_printable_DNS_request tests passed (%d out of %d)\n", i, num_DNSREQPRINT_Tests);





	/* generate_DNS_request(); */
	for (i = 0; i < num_DNSREQ_Tests;i++) {
		sresult = generate_DNS_request(test_DNSreq_hostnames[i], DNSREQ_test_ID,&size);

		if (size != expectedDNSRequestSize[i]) {
			printf("generate_DNS_request test %d failed\nexpected size:\n%d\ngot:\n%d\n", i + 1, expectedDNSRequestSize[i], size);
			printf("test passed (%d out of %d)\n", i + 1, num_DNSREQ_Tests);
			return EXIT_FAILURE;
		}
		else if (compare_DNS_requests(sresult, expectedDNSRequest[i]) == 0) {
			printf("generate_DNS_request test %d failed\nexpected:\n%s\ngot:\n%s\n", i + 1, debug_get_printable_DNS_request(expectedDNSRequest[i]), debug_get_printable_DNS_request(sresult));
			printf("test passed (%d out of %d)\n", i + 1, num_DNSREQ_Tests);
			return EXIT_FAILURE;
		}
		else {
			printf("generate_DNS_request test %d passed\n", i + 1);
		}
		free(sresult); sresult = NULL;
	}

	printf("generate_DNS_request tests passed (%d out of %d)\n", i, num_DNSREQ_Tests);

	puts("TODO: add more tests for invalid inputs.\n");

	/*if (download_file("https://github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt") != NULL) {
		puts("calling download_file without initalizing the DNS_LIST should return NULL!");
		return 1;
	}
	
*/
	DNS_LIST = malloc(sizeof(int32) * 3);
	DNS_LIST[0] = 0x08080808; /* 8.8.8.8 google dns*/
	DNS_LIST[1] = 0x08080404; /* 8.8.4.4 google dns*/
	DNS_LIST[2] = 0; /* null termination */
/*	

	if (download_file(NULL) != NULL){
		puts("passing NULL to download_file should return NULL!");
		return 1;
	}

	if (download_file("()()()") != NULL){
		puts("passing invalid url to download_file should return NULL!");
		return 1;
	}
*/

	sresult = download_file("https://github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",DNS_LIST);/**/

	for (i = 0; i < 1024; i++)
	{
		printf("%02X ", (unsigned char)sresult[i]);
	}
	printf("\n");

	return 0;
} 