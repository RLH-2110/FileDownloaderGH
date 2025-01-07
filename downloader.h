#ifndef included_downloader_h
#define included_downloader_h

#include "int.h"
#include <stdio.h>

/*
	Null terminated DNS LIST (IPV4) 
	an an ip like this 0x08080808 ( this is 8.8.8.8)
*/
extern uint32** DNS_LIST;

/*
	downloads a file from the specified url.
	DNS_LIST must be filled with a null terminated list of DNS servers before calling this. 
		DNS_LIST is a list of 32 bit integers thats terminated by null.

	URL: char* that contains the string of the url where we want to download the file
	
	log: optional log file. set to NULL if unused, or set it to point to a open file

	Returns: NULL if an error occured or a char* with the contents of the file
*/
char* download_file(char* url, int32* DNS_LIST, FILE* log);




/* 

	
	INTERNAL STUFF HERE, ONLY EXPOSED FOR TESTING!


*/

typedef struct parsedUrl {
	char* protocol;
	char* hostname;
	char* rest;
} parsedUrl;


/* 
	url: the url to normalize. this is used internally, and is only exported for testing

	returns a normalized url from the url (example: turning https://pubs.opengroup.org/onlinepubs/007904975/basedefs/sys/socket.h.html to pubs.opengroup.org
	this is just a char* srting or NULL if there was an error

	THE CALLER MUST FREE THE NORMALIZED URL!
*/
parsedUrl parse_URL(char* url);




/* takes in a hostname and returns the DNS reqeust to resolve the hostname

	output parameter: size: retuns the length of the request

	this is used internally, and is only exported for testing

	log: optional log file. set to NULL if unused, or set it to point to a open file

	returns: byte array with the dns request (CALLER MUST FREE IT!)
*/
char* generate_DNS_request(char* hostname, uint16 id, int* size, FILE* log);



/* 
	gets the QNAME from a hostname

	this is used internally, and is only exported for testing

	NOTE: THE HOSTNAME MUST BE IN A VALID FORMAT! FOR EXAMPLE: MULTIPLE DOTS AFTER EACH OTHER CAUSE UNDEFINED BEHAVIOR!

	returns a QNAME (functions simelar to a string)
	MUST BE FREED BY THE CALLER!
*/
char* getQNAME(char* hostname);

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
char* debug_get_printable_qname(char* qname);


/* print out the DNS request for debugging.

	WARNING: THIS FUNCTON WILL QUIT THE PROGAMM IN AN OUT OF MEMORY SCENARIO!
	
	WARNING: the input must point at a DNS request or NULL. Anything else will cause Undefined behavior which may cause a segfault!
	
	returns the qname, but the length values are written in ascii
	you are epected to free the pointer!
*/
char* debug_get_printable_DNS_request(char* request);

/* comprased two DNS requests
	returns 0 if they are not equal.
	returns a non zero value if they are the same.
*/
int compare_DNS_requests(char* requestA, char* requestB);

/*
	looks up the IPV4 of the provided url, using the list of dns servers.

	url: string of the url to parse. its expected that the url is in a valid format.
	DNS_LIST: a null terminated list of IPV4 addresses that are DNS servers. the addresses are saved as 32 bit integers
	log: optional log file. set to NULL if unused, or set it to point to a open stream

	returns: 0 if there was an error, or the IPV4 Address of the answer, if there was one.
*/
int32 DNS_lookup(char* url, int32* DNS_LIST, FILE* log);

/* turns the ip address to a string.
	ip: a 32 bit integer holding the ipv4 address

	returns a string formated as a decimal ip using periods as seperator.
	YOU MUST FREE IT!
	returns NULL on error
*/
char* IPv4ToString(int32 ip);

/*included_downloader_h*/
#endif