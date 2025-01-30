#ifndef included_downloader_internal_h
#define included_downloader_internal_h

#include "int.h"
#include <stdio.h>

typedef char uchar;

/* internal requirement */
#include "urlParse/urlParse.h"	

/*
	buff: buffer to the raw http response
	length: lenght of the buffer
	log: optional log file, can be set to NULL

	return: returns the size of the content that we expect or 0 on error

	errors:
		EINVAL: invalid arguments
		EIO: could not find the Content-Length
*/
uint32 httpResponseGetContentSize(unsigned char* buff, uint32 length,FILE* log);

/* takes in a hostname and returns the DNS reqeust to resolve the hostname

	output parameter: size: retuns the length of the request

	this is used internally, and is only exported for testing

	log: optional log file. set to NULL if unused, or set it to point to a open file

	returns: byte array with the dns request (CALLER MUST FREE IT!)

	errors:
		ENOMEM: could not allocate memory
*/
unsigned char* generate_DNS_request(char* hostname, uint16 id, int* size, FILE* log);



/* 
	gets the QNAME from a hostname

	this is used internally, and is only exported for testing

	NOTE: THE HOSTNAME MUST BE IN A VALID FORMAT! FOR EXAMPLE: MULTIPLE DOTS AFTER EACH OTHER CAUSE UNDEFINED BEHAVIOR!

	returns a QNAME (functions simelar to a string)
	MUST BE FREED BY THE CALLER!
*/
unsigned char* getQNAME(char* hostname);

#define mallocOrExit(ptr,size) \
	ptr = malloc(size); \
	if (ptr == NULL){ \
		puts("Out of mem!");\
		exit(EXIT_FAILURE);\
	}

/* transform a qname to a more printable format for debugging.
 
	WARNING: THIS FUNCTON WILL QUIT THE PROGAMM IN AN OUT OF MEMORY SCENARIO!

	returns the qname, but the length values are written in ascii
	you are epected to free the pointer!
*/
char* debug_get_printable_qname(unsigned char* qname);


/* print out the DNS request for debugging.

	WARNING: THIS FUNCTON WILL QUIT THE PROGAMM IN AN OUT OF MEMORY SCENARIO!
	
	WARNING: the input must point at a DNS request or NULL. Anything else will cause Undefined behavior which may cause a segfault!
	
	returns the qname, but the length values are written in ascii
	you are expected to free the pointer!
*/
char* debug_get_printable_DNS_request(unsigned char* request);

/* comprased two DNS requests
	returns 0 if they are not equal.
	returns a non zero value if they are the same.
*/
int compare_DNS_requests(unsigned char* requestA, unsigned char* requestB);

/*
	looks up the IPV4 of the provided url, using the list of dns servers.

	url: string of the url to parse. its expected that the url is in a valid format.
	DNS_LIST: a null terminated list of IPV4 addresses that are DNS servers. the addresses are saved as 32 bit integers
	log: optional log file. set to NULL if unused, or set it to point to a open stream

	returns: 0 if there was an error, or the IPV4 Address of the answer, if there was one.

	errors:
		ENOMEM: there was no memory left.
		EIO: socket or WSAStartup failed
		EINVAL: invalid arguments passed
*/
int32 DNS_lookup(char* url, int32* DNS_LIST, FILE* log);

/* parses a DNS repsonse and turns it into an IP

	invalid urls will result in undefined behavior!

	DNS_response: char pointer pointing to the first byte of the response
	id:	expected return ID from the DNS response
	recv_len: the maximum length of DNS_response, used to guard against segmentation faults
	log: optional logging file, set to NULL if unused

	returns the ip if one could be found otherwhise it returns 0
*/
int32 DNS_parse_reply(unsigned char* DNS_response, int16 id, int recv_len, FILE* log);


/* turns the ip address to a string.
	ip: a 32 bit integer holding the ipv4 address

	returns a string formated as a decimal ip using periods as seperator.
	YOU MUST FREE IT!
	returns NULL on error
*/
char* IPv4ToString(int32 ip);

/* same as above, but it will reverse the byte order first, so that you can print ip adresses that are in network order*/
char* IPv4ToStringR(int32 ip);

/* included_downloader_internal_h */
#endif