#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "downloader.h"
#include "int.h"
#include <time.h>
#include <errno.h>

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef true
typedef int bool;
#define true 0xFF
#define false 0x0
#endif

#define putslog(str)\
if(log != NULL){\
	fputs(str,log);\
	fputs("\n",log);\
}
#define perrorlog(str)\
if(log != NULL){\
	fputs(str,log);\
	fputs(" ",log);\
	fputs(strerror(errno),log);\
	fputs("\n",log); \
}


enum trimState {
	TS_findDot,
	TS_trimStart,
	TS_findEnd,
	TS_exit
};

char* IPv4ToString(int32 ip){

	char* str = malloc(4*4); /* 4*4, because it is 4 times maximal 3 digits plus a seperator or NULL*/
	if (str == NULL)
		return NULL;

	sprintf(str,"%d.%d.%d.%d", (ip & 0xFF000000) >> 24 , (ip & 0x00FF0000) >> 16, (ip & 0x0000FF00) >> 8, ip & 0x000000FF);
	return str;
}

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

	returns a QNAME
	caller frees

	more detail in the header
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

char* debug_get_printable_qname(char* qname) {

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
			sprintf(buff, "%d", length);

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
#define DNS_QUESTION_SIZE_ADJ 4
#define RQEUEST_SIZE DNS_HEADER_SIZE + DNS_QUESTION_SIZE_ADJ

/* https://mislove.org/teaching/cs4700/spring11/handouts/project1-primer.pdf */
/* takes in a hostname and returns the DNS reqeust to resolve the hostname

	output parameter: size: retuns the length of the request
	
	returns: byte array with the dns request (CALLER MUST FREE IT!)
*/
char* generate_DNS_request(char* hostname, uint16 id, int* size, FILE* log) {

	int request_index;
	char* qname;
	char* request;

#define ADJUSTED_REQUEST_SIZE RQEUEST_SIZE + strlen(hostname) + 2

	*size = ADJUSTED_REQUEST_SIZE;
	request = malloc(ADJUSTED_REQUEST_SIZE + 1); /* add the lentgh of the hostname, since that is dynamic. add 2 because the qname adds 2 more bytes */
	if (request == NULL)
		return NULL;

	request[ADJUSTED_REQUEST_SIZE] = (char)0xAA;
	

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

	#define DNS_FLAGS (DNS_RCODE | (DNS_Z << 4) | (DNS_RA << 7) | (DNS_RD << 8) | (DNS_TC << 9) | (DNS_AA << 10) | (DNS_OPCODE << 11) | (DNS_QR << 15))

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



/* the null terminator is important, since it signals the end of the qname */
#define qnameLen (strlen(qname)+1)
	qname = getQNAME(hostname);
	
#define reqIndexSTART 12
	request_index = reqIndexSTART; /* ar count ended with index 11, so now new data is on index 12*/
	
	/* copy from the qname to the request. */
	for (;request_index - reqIndexSTART < qnameLen;request_index++) {
		request[request_index] = qname[request_index - reqIndexSTART];
	}
	free(qname); qname = NULL;


	/* QTYPE */
	request[request_index+0] = 0;
	request[request_index+1] = 1;

	/* QCLASS */
	request[request_index+2] = 0;
	request[request_index+3] = 1;

	if(request[ADJUSTED_REQUEST_SIZE] != (char)0xAA) {
		putslog("OOPS, I might have corupted memory in generate_DNS_request. Exiting progamm...");
		exit(EXIT_FAILURE);
	}

	return request;
}

#define printable_DNS_request_format \
"ID: %d\nQDCOUNT: %d\nANCOUNT: %d\nNSCOUNT: %d\nARCOUNT: %d\nFLAGS:\n\tQR: %d\n\tOpcode: %d\n\tAA: %d\n\tTC: %d\n\tRD: %d\n\tRA: %d\n\tZ: %d\n\tRCODE: %d\nQNAME: %s\nQTYPE: %d\nQCLASS: %d\n"

#define longest_printable_DNS_request_with_empty_QNAME \
"ID: 65536\nQDCOUNT: 65536\nANCOUNT: 65536\nNSCOUNT: 65536\nARCOUNT: 65536\nFLAGS:\n\tQR: 1\n\tOpcode: 15\n\tAA: 1\n\tTC: 1\n\tRD: 1\n\tRA: 1\n\tZ: 1\n\tRCODE: 15\nQNAME: \nQTYPE: 65536\nQCLASS: QCLASS\n"

char* debug_get_printable_DNS_request(char* request) {

	char* result;
	int request_index;

	/* header */
	uint16 id;
	uint16 flags;
	uint16 qdcount;
	uint16 ancount;
	uint16 nscount;
	uint16 arcount;

	/* question */
	char* qname;
	uint16 qtype;
	uint16 qclass;

	if (request == NULL) {
		mallocOrExit(result, 5);
		strcpy(result, "NULL");
		return result;
	}
	
	id		= (request[0] << 8) + request[1];
	flags	= (request[2] << 8) + request[3];
	qdcount	= (request[4] << 8) + request[5];
	ancount	= (request[6] << 8) + request[7];
	nscount	= (request[8] << 8) + request[9];
	arcount	= (request[10]<< 8) + request[11];

	request_index = reqIndexSTART;
	qname = debug_get_printable_qname(request+request_index);

	for (;request[request_index] != 0;request_index++) {
		/* just wait till we hit the NULL terminator for the QNAME*/
	}
	request_index++; /*now move past the NULL terminator*/

	qtype = (request[request_index+0] << 8) + request[request_index+1];
	qclass = (request[request_index+2] << 8) + request[request_index+3];

	request_index += 3; /* save the offset where the request ends. */

	mallocOrExit(result,(strlen(qname) + strlen(longest_printable_DNS_request_with_empty_QNAME) + 5)); /* string lenght of longest request with empty qname, lengh of qname and 5 exta buffer */

	if (
		sprintf(result, printable_DNS_request_format, id, qdcount, ancount, nscount, arcount,
			flags >> 15 & 0x1,
			flags >> 11 & 0x7,
			flags >> 10 & 0x1,
			flags >> 9 & 0x1,
			flags >> 8 & 0x1,
			flags >> 7 & 0x1,
			flags >> 4 & 0x1,
			flags >> 0 & 0x7,
			qname,
			qtype,
			qclass
		)
		== -1) 
	{
		mallocOrExit(result, 7);
		strcpy(result, "ERROR!");
	}
	
	free(qname); qname = NULL;
	return result;
}

int compare_DNS_requests(char* requestA, char* requestB) {
	int index = 0;

	if (requestA == NULL && requestB == NULL) {
		return 0xff;
	}

	if (requestA == NULL || requestB == NULL) {
		return 0;
	}

	/* compare before the QNAME*/
	while (index < reqIndexSTART) {
		if (requestA[index] != requestB[index])
			return 0;
		index++;
	}

	/* compare the QNAME */
	while (requestA[index] != 0 && requestB[index] != 0) {
		if (requestA[index] != requestB[index])
			return 0;
		index++;
	}

	/* check if both QNAMEs terminate */
	if (requestA[index] != requestB[index])
		return 0;
	index++;

	/* check the last 4 bytes */
	if (requestA[index+0] != requestB[index+0])
		return 0;
	if (requestA[index+1] != requestB[index+1])
		return 0;
	if (requestA[index+2] != requestB[index+2])
		return 0;
	if (requestA[index+3] != requestB[index+3])
		return 0;

	return 0xFF;

}

#ifdef POSIX

/*  https://pubs.opengroup.org/onlinepubs/007904975/basedefs/sys/socket.h.html */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

int32 DNS_lookup(char* url, int32* DNS_LIST, FILE* log){

	struct timeval tv;
	struct sockaddr_in server_addr;
	int ret;
	char* DNS_request;
	parsedUrl p_url;
	uint16 id;
	int request_size;
	int sock;
	int DNSindex; /* the DNS_LIST index*/
	socklen_t address_len = sizeof(struct sockaddr_in);

	if (url == NULL || DNS_LIST == NULL){
		perrorlog("url and DNS_LIST must not be zero!");
		return 0;
	}

	if (DNS_LIST[0] == 0){
		perrorlog("DNS_LIST is emptry");
		return 0;
	}
	DNSindex = 0;



	if (!true){ /* never go here normally */

retry_with_new_DNS:
		close(sock); /* close socket */
		DNSindex++;
		if (DNS_LIST[DNSindex] == 0){
			putslog("Can not connect to any dns in the list!");
			return 0;
		}
	}	


	
	sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); 
	if (sock < 0) {
    	perrorlog("socket failed");
    	return 0;
	}

	/* setting socket options */
	tv.tv_sec = 0;
	tv.tv_usec = 500000; /* this is in microseconds and equals to 500 ms) */
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) != 0){
		perrorlog("setting socket timeout failed");
		close(sock);
		return 0;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(53); /* DNS port */
	server_addr.sin_addr.s_addr = htonl(DNS_LIST[DNSindex]); 

	putslog("debug: Trying to connect...");

	ret = connect(sock, (struct sockaddr*)&server_addr, address_len);
	if (ret != 0){
		perrorlog("connect failed");
		goto retry_with_new_DNS;
	}

	id = (uint16)time(NULL);
	p_url = parse_URL(url);
	DNS_request = generate_DNS_request(p_url.hostname, id, &request_size, log);


	putslog("debug: Trying send...");

	if (sendto(sock, DNS_request, request_size, MSG_EOR, (struct sockaddr*)&server_addr, address_len) == -1) {
		perrorlog("error while doing sentto");
		goto DNS_lookup_cleanup;
	}
	free(DNS_request);
		

	putslog("debug: Trying to recive...");

	errno = 0;

#define recv_len 1024
	DNS_request = malloc(recv_len); /* 1 KiB ought to be enough for everyone*/
	if (recvfrom(sock, DNS_request, recv_len, MSG_WAITALL, (struct sockaddr*)&server_addr, &address_len) == -1) {

		if (errno == EAGAIN || errno == EWOULDBLOCK){ /* Timeout */
			free(DNS_request);
			goto retry_with_new_DNS;
		}

		perrorlog("error while doing recvfrom");
		goto DNS_lookup_cleanup;
	}

	putslog("debug: done for now.");

	close(sock);

	putslog("temporary debug result as return value! fix it later!");
	return 5;

DNS_lookup_cleanup:
	free(DNS_request);
	close(sock);
	return 0;
}

char* download_file(char* url, int32* DNS_LIST, FILE* log){
	puts("Not implemented");
	return NULL;
}

/* POSIX */
#endif


#ifdef WINDOWS

#endif

