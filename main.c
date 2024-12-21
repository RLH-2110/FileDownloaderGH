#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "downloader.h"
#include "int.h"

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

	parsedUrl parsed;
	parsedUrl returnNull;

	char* normalized_url = NULL;
	char* rest = NULL;
	char* protocol = NULL;

	int dotIndex, currentIndex, endIndex, startIndex;
	int dots = 0;
	bool foundDot = false; /* if we find a dot when this is true, then that means we have 2 dots next to each other*/

	int state = TS_findDot;
	currentIndex = 0;

	returnNull.protocol = NULL;
	returnNull.hostname = NULL;
	returnNull.rest = NULL;

	if (url == NULL)
		return returnNull; /* filled with NULL */


	while (state != TS_exit) {

		switch (state) {
			case TS_findDot:
			{
				if (url[currentIndex] == 0)
					return returnNull; /* filled with NULL */

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
					return returnNull; /* filled with NULL */

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
						return returnNull; /* filled with NULL */ /* invalid url when 2 dots are next to each other. */
					foundDot = true;
				
					currentIndex++;
					break;
				}

				if (url[currentIndex] == '/') {
					currentIndex--;
				TS_findEnd_nextSate:
					endIndex = currentIndex;
					state = TS_exit;
					currentIndex = dotIndex;
					break;
				}

				currentIndex++;
				foundDot = false;
				break;
			}

		}

	}
 




	/*fix subdomain*/

	/* +2 because index is always one smaller, and we need a null byte*/
#define normalizedMallocSsize endIndex + 2 - startIndex 
	if (dots >= 2) {

		normalized_url = malloc(normalizedMallocSsize);
		if (normalized_url == NULL)
			return returnNull;

		memcpy(normalized_url, url + startIndex, normalizedMallocSsize);
		normalized_url[normalizedMallocSsize - 1] = 0;



	}
	else {

		normalized_url = malloc(normalizedMallocSsize + 4); /* +4 because we need to add www. */
		if (normalized_url == NULL)
			return returnNull;

		memcpy(normalized_url + 4, url + startIndex, normalizedMallocSsize); /* +4 because www. is missing*/
		normalized_url[0] = 'w';
		normalized_url[1] = 'w';
		normalized_url[2] = 'w';
		normalized_url[3] = '.';
		normalized_url[normalizedMallocSsize + 4 - 1] = 0;



	}

	/* fix protocol */
	if (startIndex - 3 > 0) {
		protocol = malloc(startIndex - 3 + 1);
		if (protocol == NULL)
			return returnNull;

		memcpy(protocol, url, startIndex - 3);
		protocol[startIndex - 3] = 0;
	}
	else {
		/* allocated, so we can just free it, instead of worring what happends if we free memory that we never allocated*/
		protocol = malloc(6);
		if (protocol == NULL)
			return returnNull;
		protocol[0] = 'h'; protocol[1] = 't'; protocol[2] = 't'; protocol[3] = 'p'; protocol[4] = 's'; protocol[5] = 0;

	}

	rest = malloc(strlen(url) + 1 - endIndex);
	if (rest == NULL)
		return returnNull;
	memcpy(rest, url + endIndex + 1, strlen(url) + 1 - endIndex);
	rest[strlen(url) - endIndex] = 0;

	parsed.protocol = protocol;
	parsed.hostname = normalized_url;
	parsed.rest = rest;


	return parsed;

}


/* 
	gets the QNAME from a hostname

	returns a QNAME as a string (\0 terminated)
	IMPORTANT: the actuall qname is 0x30 '0' terminated, we have that terminator, but we added a \0 so we can get the total length with the strlen function.
*/
char* getQNAME(char* hostname){

	char* qname;

	uint8 length ;
	char* section_curr;
	char* qname_current;
	char* qname_lentgh;

	qname = malloc(strlen(hostname)+2); /* the "dots" will be replaced by numbers. ie. www.google.de -> 3www6google2de0  for the first and last one we have to add 2 bytes. */
	if (qname == NULL)
		return NULL;

	length = 0;
	qname_current = qname + 1;
	qname_lentgh = qname;
	section_curr = hostname;

	while(*section_curr != 0){

		if (*section_curr == '.'){

			*qname_lentgh = length; /* write the lentgh before we start the text section*/
			length = 0; /* restet length */
			qname_lentgh = qname_current; /* our current position is the dot, that will be the next index where we write the lentgh*/

		}else{
			length++;
			if (length == 0) /* overflow happened - BAD */
				return NULL; 

			*qname_current = *section_curr; /* copy the data */
		}

		qname_current++;
		section_curr++;
	}
	*qname_lentgh = length; /* write the lentgh of the last section*/

	*qname_current = 0;
	return qname;
}

char* debug_print_qname(char* qname) {

	char* result;
	int state = 0;
	unsigned char length;
	char* buff;
	int i;

	char* resultp;

	if (qname == NULL) {
		mallocOrExit(result, 5);
		strcpy(result, "NULL");
		return result;
	}
	if (*qname == 0) {
		mallocOrExit(result, 1);
		strcpy(result, "");
		return result;
	}

	mallocOrExit(result, strlen(qname) * 3);
	resultp = result;
	mallocOrExit(buff, 4);

	while (*qname != 0) {

		switch (state)
		{
		case 0:
			length = *qname;
			snprintf(buff, 4, "%d", length);

			for (i = 0;i < strlen(buff);i++)
				*resultp++ = buff[i];

			state = 1;
			break;
		case 1:
			*resultp = *qname;
			resultp++;
			if (--length == 0)
				state = 0;
		}
		qname++;
	}
	*resultp = 0;
	return result;
}

#define DNS_HEADER_SIZE 12
#define DNS_QUESTION_SIZE_ADJ 0
#define RQEUEST_SIZE DNS_HEADER_SIZE + DNS_QUESTION_SIZE_ADJ

/* https://mislove.org/teaching/cs4700/spring11/handouts/project1-primer.pdf */
/* takes in a hostname and returns the DNS reqeust to resolve the hostname

	returns: byte array with the dns request (CALLER MUST FREE IT!)
*/
char* generate_DNS_request(char* hostname, uint16 id) {

	char* request = malloc(RQEUEST_SIZE + strlen(hostname) + 1); /* add the lentgh of the hostname, since that is dynamic */
	if (request == NULL)
		return NULL;


	/*  /------------\
		| DNS HEADER |
	    \------------/  */

	/* https://mislove.org/teaching/cs4700/spring11/handouts/project1-primer.pdf */
	#define DNS_QR 0
	#define DNS_OPCODE 0 
	#define DNS_AA 0 
	#define DNS_TC 0
	#define DNS_RD 1
	#define DNS_RA 0
	#define DNS_Z  0
	#define DNS_RCODE 0

	#define DNS_FLAGS DNS_RCODE | (DNS_Z << 4) | (DNS_RA << 7) | (DNS_RD << 8) | (DNS_TC << 9) | (DNS_AA << 10) | (DNS_OPCODE << 11) | (DNS_QR << 15)

	/* id */
	request[0] = (uint8)(id >> 8); /* load upper half of the id */
	request[1] = (uint8)id;		   /* load lowe half of the id  */
	
	/* flags */
	request[2] = (uint8)(DNS_FLAGS >> 8);
	request[3] = (uint8)(DNS_FLAGS);

	/* QD count */
	request[4] = 0;
	request[5] = 1;

	/* AN count */
	request[6] = 0;
	request[7] = 0;

	/* MS count */
	request[8] = 0;
	request[9] = 0;

	/* AR count */
	request[10] = 0;
	request[11] = 0;


	/*  /--------------\
		| DNS QUESTION |
	    \--------------/  */



	/* TODO */


	request[RQEUEST_SIZE] = 0; /* null terminated, so we cant print this thing as a string if we wish. */
	return request;
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

