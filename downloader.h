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

	URL: char* that contains the string of the url where we want to download the file
	
	Returns: NULL if an error occured or a char* with the contents of the file
*/
char* download_file(char* url);



/* 
	url: the url to normalize. this is used internally, and is only exported for testing

	returns a normalized url from the url (example: turning https://pubs.opengroup.org/onlinepubs/007904975/basedefs/sys/socket.h.html to pubs.opengroup.org
	this is just a char* srting or NULL if there was an error

	THE CALLER MUST FREE THE NORMALIZED URL!
*/
char* trim_URL(char* url);

/*included_downloader_h*/
#endif