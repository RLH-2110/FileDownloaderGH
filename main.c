#include <stdlib.h>
#include <string.h>
#include "downloader.h"

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef true
typedef int bool;
#define true 0xFF
#define false 0X0
#endif

enum trimState {
	TS_findDot,
	TS_trimStart,
	TS_findEnd,
	TS_fillStruct,
	TS_exit
};

/*
	url: the VALID url to parse. this will not fix broken urls
	do not use leading or tailing dots

	returns an struct that turns the url into 3 parts. 
		for example this url: https://pubs.opengroup.org/onlinepubs/007904975/basedefs/sys/socket.h.html would be split into https   ,   pubs.opengroup.org     and     /onlinepubs/007904975/basedefs/sys/socket.h.html
	it will return an array filled with NULL if there was an errror.

	THE CALLER MUST FREE THE EVERY STRING IN THE STUCT!!!!
*/
parsedUrl parse_URL(char* url) {

	parsedUrl parsed = { .protocol = NULL, .hostname = NULL, .rest = NULL };
	
	char* normalized_url = NULL;
	char* rest = NULL;
	char* protocol = NULL;

	int dotIndex, currentIndex, endIndex, startIndex;
	int dots = 0;
	bool foundDot = false; /* if we find a dot when this is true, then that means we have 2 dots next to each other*/

	int state = TS_findDot;
	currentIndex = 0;

	if (url == NULL)
		return parsed; /* filled with NULL */


	while (state != TS_exit) {

		switch (state) {


		case TS_findDot:
		{
			if (url[currentIndex] == 0)
				return parsed; /* filled with NULL */

			if (url[currentIndex] != '.') {
				currentIndex++;
				break;
			}
			/* found a . */
			dots++;
			dotIndex = currentIndex;
			state = TS_trimStart;
			foundDot = true;
			currentIndex--;
			break;
		}



		case TS_trimStart:
		{
			if (currentIndex < 0)
				return parsed; /* filled with NULL */

			if (currentIndex == 0)
				goto TS_trimStart_nextSate;

			if (url[currentIndex] == '/') {
				currentIndex++;
			TS_trimStart_nextSate:
				startIndex = currentIndex;
				state = TS_findEnd;
				currentIndex = dotIndex + 1;
				break;
			}

			currentIndex--;
			break;
			}

		case TS_findEnd:
		{
			if (url[currentIndex] == 0)
				goto TS_findEnd_nextSate;

			if (url[currentIndex] == '.') {
				dots++;
				if (foundDot == true)
					return parsed; /* filled with NULL */ /* invalid url when 2 dots are next to each other. */
				foundDot = true;
				
				currentIndex++;
				break;
			}

			if (url[currentIndex] == '/') {
				currentIndex--;
			TS_findEnd_nextSate:
				endIndex = currentIndex;
				state = TS_fillStruct;
				currentIndex = dotIndex;
				break;
			}

			currentIndex++;
			foundDot = false;
			break;
		}

		case TS_fillStruct:
		{

			//fix subdomain

			/* +2 because index is always one smaller, and we need a null byte*/
#define normalizedMallocSsize endIndex + 2 - startIndex 
			if (dots >= 2) {

				normalized_url = malloc(normalizedMallocSsize);
				if (normalized_url == NULL)
					return (parsedUrl) { NULL, NULL, NULL };

				memcpy(normalized_url, url + startIndex, normalizedMallocSsize);
				normalized_url[normalizedMallocSsize - 1] = 0;



			}
			else {

				normalized_url = malloc(normalizedMallocSsize + 4); /* +4 because we need to add www. */
				if (normalized_url == NULL)
					return (parsedUrl) { NULL, NULL, NULL };

				memcpy(normalized_url+4, url + startIndex, normalizedMallocSsize); /* +4 because www. is missing*/
				normalized_url[0] = 'w';
				normalized_url[1] = 'w';
				normalized_url[2] = 'w';
				normalized_url[3] = '.';
				normalized_url[normalizedMallocSsize + 4 - 1] = 0;

				

			}

			//fix protocol
			if (startIndex - 3 > 0) {
				protocol = malloc(startIndex - 3 + 1);
				if (protocol == NULL)
					return (parsedUrl) { NULL, NULL, NULL };

				memcpy(protocol, url, startIndex - 3);
				protocol[startIndex - 3] = 0;
			}
			else {
				// allocated, so we can just free it, instead of worring what happends if we free memory that we never allocated
				protocol = malloc(6);
				if (protocol == NULL)
					return (parsedUrl) { NULL, NULL, NULL };
				protocol[0] = 'h'; protocol[1] = 't'; protocol[2] = 't'; protocol[3] = 'p'; protocol[4] = 's'; protocol[5] = 0;

			}

			rest = malloc(strlen(url) + 1 - endIndex);
			if (rest == NULL)
				return (parsedUrl) { NULL, NULL, NULL };
			memcpy(rest, url + endIndex + 1, strlen(url) + 1 - endIndex);

			parsed.protocol = protocol;
			parsed.hostname = normalized_url;
			parsed.rest = rest;

			state = TS_exit;
			break;
		}



		}


	}
 /* todo: make finish*/
	return parsed;

}


char* generate_DNS_restuest(char* normalized_url) {
	return NULL;
}


#ifdef POSIX

/*  https://pubs.opengroup.org/onlinepubs/007904975/basedefs/sys/socket.h.html */
#include <sys/socket.h>


char* download_file(char* url){
/*
	struct sockaddr_in server_addr;
	int ret;

	if (url == NULL || DNS_LIST == NULL){
		perror("url and DNS_LIST must not be zero!");
		return NULL;
	}

	if (DNS_LIST[0] == 0){
		perror("DNS_LIST is emptry")
		return NULL;
	}


	/* UNTESTED CODE STARTS HERE*/

	
/*	int sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); 
	if (sock < 0) {
    	perror("socket failed");
    	return NULL;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(53); /* DNS port */
/*	server_addr.sin_addr.s_addr = htonl(DNS_LIST[0]); 

	ret = connect(sock,&server_addr,sizeof(struct sockaddr_in));
	if (ret != 0){
		perror("connect failed");
		return NULL;
	}

	/* read this and then continue
	/ https://mislove.org/teaching/cs4700/spring11/handouts/project1-primer.pdf
	*/

	return "hello world";
}

/* POSIX */
#endif


#ifdef WINDOWS

#endif

