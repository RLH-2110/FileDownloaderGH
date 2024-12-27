#ifndef included_downloader_h
#define included_downloader_h

#include "int.h"

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
	
	Returns: NULL if an error occured or a char* with the contents of the file
*/
char* download_file(char* url, int32* DNS_LIST);




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

	returns: byte array with the dns request (CALLER MUST FREE IT!)
*/
char* generate_DNS_request(char* hostname, uint16 id, int* size);



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

/*included_downloader_h*/
#endif