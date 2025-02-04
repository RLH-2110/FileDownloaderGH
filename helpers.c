#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "int.h"

#include "DNS_offsets.h"
#include "defines.h"


/* these are just here for IPv4ToStringR, because I am to lazy to reverse the bytes myself (I tried to, but I am to lazy to fix my broken mess)*/
#ifdef POSIX
#include <arpa/inet.h>
#endif
#ifdef WINDOWS
#include <winsock2.h>
#endif


/*

	This file is mostly for printing and compareing functions

*/



/* turns the ip address to a string.
	ip: a 32 bit integer holding the ipv4 address

	returns a string formated as a decimal ip using periods as seperator.
	YOU MUST FREE IT!
	returns NULL on error
*/
static char* IPv4ToString(int32 ip){

	char* str = malloc(4*4); /* 4*4, because it is 4 times maximal 3 digits plus a seperator or NULL*/
	if (str == NULL)
		return NULL;

	sprintf(str,"%d.%d.%d.%d", (ip & 0xFF000000) >> 24 , (ip & 0x00FF0000) >> 16, (ip & 0x0000FF00) >> 8, ip & 0x000000FF);
	return str;
}
/* same as IPv4ToString, but it will reverse the byte order first, so that you can print ip adresses that are in network order*/
static char* IPv4ToStringR(int32 ip){
	int32 neoIP;

	neoIP = htonl(ip); /* reverse it */
	return IPv4ToString(neoIP);
}

/* transform a qname to a more printable format for debugging.
 
	WARNING: THIS FUNCTON WILL QUIT THE PROGAMM IN AN OUT OF MEMORY SCENARIO!

	returns the qname, but the length values are written in ascii
	you are epected to free the pointer!
*/
static char* debug_get_printable_qname(unsigned char* qname) {

	char* result;
	int state = 0;
	unsigned char length;
	char* buff;
	unsigned int i;

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

	mallocOrExit(result, strlen((char*)qname) * 3);
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


#define printable_DNS_request_format \
"ID: %d\nQDCOUNT: %d\nANCOUNT: %d\nNSCOUNT: %d\nARCOUNT: %d\nFLAGS:\n\tQR: %d\n\tOpcode: %d\n\tAA: %d\n\tTC: %d\n\tRD: %d\n\tRA: %d\n\tZ: %d\n\tRCODE: %d\nQNAME: %s\nQTYPE: %d\nQCLASS: %d\n"

#define longest_printable_DNS_request_with_empty_QNAME \
"ID: 65536\nQDCOUNT: 65536\nANCOUNT: 65536\nNSCOUNT: 65536\nARCOUNT: 65536\nFLAGS:\n\tQR: 1\n\tOpcode: 15\n\tAA: 1\n\tTC: 1\n\tRD: 1\n\tRA: 1\n\tZ: 1\n\tRCODE: 15\nQNAME: \nQTYPE: 65536\nQCLASS: QCLASS\n"


/* print out the DNS request for debugging.

	WARNING: THIS FUNCTON WILL QUIT THE PROGAMM IN AN OUT OF MEMORY SCENARIO!
	
	WARNING: the input must point at a DNS request or NULL. Anything else will cause Undefined behavior which may cause a segfault!
	
	returns the qname, but the length values are written in ascii
	you are expected to free the pointer!
*/
static char* debug_get_printable_DNS_request(unsigned char* request) {

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

	request_index = DNS_QUESTION_OFFSET;
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


/* compares two DNS requests
	returns 0 if they are not equal.
	returns a non zero value if they are the same.
*/
static int compare_DNS_requests(unsigned char* requestA, unsigned char* requestB) {
	int index = 0;

	if (requestA == NULL && requestB == NULL) {
		return 0xff;
	}

	if (requestA == NULL || requestB == NULL) {
		return 0;
	}

	/* compare before the QNAME*/
	while (index < DNS_QUESTION_OFFSET) {
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