#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <errno.h>

#include "int.h"

#include "downloader.h"
#include "DNS_offsets.h"
#include "defines.h"


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


#define DNS_HEADER_SIZE 12
#define DNS_QUESTION_SIZE_ADJ 4
#define RQEUEST_SIZE DNS_HEADER_SIZE + DNS_QUESTION_SIZE_ADJ

/* https://mislove.org/teaching/cs4700/spring11/handouts/project1-primer.pdf */
/* takes in a hostname and returns the DNS reqeust to resolve the hostname

	output parameter: size: retuns the length of the request
	
	returns: byte array with the dns request (CALLER MUST FREE IT!)
*/
char* generate_DNS_request(char* hostname, uint16 id, int* size, FILE* log) {

	uint16 dns_flags;
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
	dns_flags = 0;
	DNS_FLAGS_SET_QR(		dns_flags,0);
	DNS_FLAGS_SET_OPCODE(	dns_flags,0);
	DNS_FLAGS_SET_AA(		dns_flags,0);
	DNS_FLAGS_SET_TC(		dns_flags,0);
	DNS_FLAGS_SET_RD(		dns_flags,1);
	DNS_FLAGS_SET_RA(		dns_flags,0);
	DNS_FLAGS_SET_Z(		dns_flags,0);
	DNS_FLAGS_SET_RCODE(	dns_flags,0);
	
	/* id */
	request[DNS_HEADER_ID_OFFSET]   = (uint8)(id >> 8); /* load upper half of the id */
	request[DNS_HEADER_ID_OFFSET+1] = (uint8)id;		/* load lowe half of the id  */
	
	/* flags */
	request[DNS_HEADER_FLAGS_OFFSET]   = (uint8)(dns_flags >> 8);
	request[DNS_HEADER_FLAGS_OFFSET+1] = (uint8)(dns_flags);

	/* QD count */
	request[DNS_HEADER_QDCOUNT_OFFSET]   = 0;
	request[DNS_HEADER_QDCOUNT_OFFSET+1] = 1;

	/* AN count */
	request[DNS_HEADER_ANCOUNT_OFFSET]   = 0;
	request[DNS_HEADER_ANCOUNT_OFFSET+1] = 0;

	/* MS count */
	request[DNS_HEADER_MSCOUNT_OFFSET]   = 0;
	request[DNS_HEADER_MSCOUNT_OFFSET+1] = 0;

	/* AR count */
	request[DNS_HEADER_ARCOUNT_OFFSET]   = 0;
	request[DNS_HEADER_ARCOUNT_OFFSET+1] = 0;


	/*  /--------------\
		| DNS QUESTION |
	    \--------------/  */



/* the null terminator is important, since it signals the end of the qname */
#define qnameLen (strlen(qname)+1)
	qname = getQNAME(hostname);
	
	request_index = DNS_QUESTION_OFFSET; 
	
	/* copy from the qname to the request. */
	for (;request_index - DNS_QUESTION_OFFSET < qnameLen;request_index++) {
		request[request_index] = qname[request_index - DNS_QUESTION_OFFSET];
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



int32 DNS_parse_reply(char* DNS_response){

	uint16 dns_answers;
	int dns_answers_index;
	int i;
	uint16 flags;
	int state;
	uint16 rdlength;
	uint32 ip_address;
	uint16 dns_questions;



	flags = DNS_construct_flags(DNS_response[DNS_HEADER_FLAGS_OFFSET],DNS_response[DNS_HEADER_FLAGS_OFFSET+1]);

	/* check if the ID is the same */
	if (DNS_response[DNS_HEADER_ID_OFFSET] != (char)((id & 0xFF00)>>8) || DNS_response[DNS_HEADER_ID_OFFSET+1] != (char)(id & 0xFF)){

		printf("\nresponse upper:\t %d\n excpected upper:\n %d\n\nresponse lower:\t %d\nexpected lower:\t %d\n",DNS_response[DNS_HEADER_ID_OFFSET],((id & 0xFF00)>>8),DNS_response[DNS_HEADER_ID_OFFSET+1],(id & 0xFF));

		putslog("The IDs not match!");
		return 0;
	}
	
	/* check if its actually an answer*/
	if (DNS_FLAGS_GET_QR(flags) != 1){
		putslog("The DNS responed with a question!");
		return 0;
	}

	/* check if there was no error */
	if(DNS_FLAGS_GET_RCODE(flags) != 0){
		switch(DNS_FLAGS_GET_RCODE(flags)){
		case 1:
			putslog("CRITICAL! DNS RESPONEDED WITH 'FORMAT ERROR' IN THE RCODE!"); break;

		case 2:
			putslog("DNS responded with 'Server failure' in RCODE"); break;
		
		case 3:
			putslog("DNS responed with 'Name Error' in RCODE"); break;
		
		case 4:
			putslog("DNS responed with 'Not Implemented' in RCODE"); break;
		
		case 5:
			putslog("DNS responed with 'Refused' in RCODE"); break;
			
		default:
			printflog("DNS responed with unrecognized RCODE: %d\n",DNS_FLAGS_GET_RCODE(DNS_response[DNS_HEADER_FLAGS_OFFSET])); break;
		}

		return 0;
	}

	/* check that its not trunkated */
	if(DNS_FLAGS_GET_TC(flags) != 0){
		putslog("The DNS responed with tuncation!");
		return 0;
	}

	dns_answers = DNS_response[DNS_HEADER_ANCOUNT_OFFSET] << 8;
	dns_answers |= DNS_response[DNS_HEADER_ANCOUNT_OFFSET+1];
	
	printflog("DNS has %d answers\n",dns_answers);

	putslog("respone: ");
	for (i = 0; i < 1024; i++)
	{
		printflog("%02X ", (unsigned char)DNS_response[i]);
	}
	printflog("\n");

	i = DNS_HEADER_SIZE; /* this should now point at the first byte of the DNS response */



	/* handle if the server returns dns questions */
	dns_questions = DNS_response[DNS_HEADER_QDCOUNT_OFFSET+1] + (DNS_response[DNS_HEADER_QDCOUNT_OFFSET] << 8);

	for(;dns_questions > 0;dns_questions--){
		/* skip though the name */
		/* note: the url we asked for is stored in p_url.hostname. if we want to, we can check if the answer we got matches what we asked for*/
		while(DNS_response[i] != 0){
			
			if (DNS_response[i] & 0xC000) /* pointer */
				break;
			else
				i += DNS_response[i] + 1; /* got to next lenght specifier*/
		}

		i += 5; /* go to the next section*/
	}



	/* handle DNS responses */
	for (dns_answers_index = 0; dns_answers_index < dns_answers && dns_answers_index < recv_len; dns_answers_index++){

		/* skip though the name */
		while(DNS_response[i] != 0){
			
			if (DNS_response[i] & 0xC000){ /* pointer */
				i++;
				break;
			}else
				i += DNS_response[i] + 1; /* got to next lenght specifier*/
			
		}
		i++; /* go past the name */

		/* check TYPE */
		if (DNS_response[i] == 0 && DNS_response[i+1] == 1){ /* if we found an A record */
			state = DLS_found;
		}else{
			state = DLS_skipping;
		}
		i += 8; /* skip Type and TTL */
		
		rdlength = DNS_response[i+1] | (DNS_response[i] << 8); i += 2;

		if (state == DLS_skipping || rdlength != 4){ /* we are either not interested in the data, or the data is NOT a ipv4*/
			i += rdlength;
		}else{
			/*This is the IPV4 we want!*/

			ip_address = (DNS_response[i+3] <<  0);
			ip_address |= (DNS_response[i+2] <<  8);
			ip_address |= (DNS_response[i+1] << 16);
			ip_address |= (DNS_response[i+0] << 24);

			return ip_address;
		}
	}

	/* we dit not find any results */
	return 0;

}