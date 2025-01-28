#ifndef included_downloader_h
#define included_downloader_h

#include "int.h"
#include <stdio.h>


/*
	downloads a file from the specified url.
	DNS_LIST must be filled with a null terminated list of DNS servers before calling this. 
		DNS_LIST is a list of 32 bit integers thats terminated by null.

	URL: char* that contains the string of the url where we want to download the file
	
	port: the port to connect to, something like 80 or 443

	out_fileSize: pointer to where you want to store the lenght of the donwloaded data

	log: optional log file. set to NULL if unused, or set it to point to a open file

	Returns: NULL if an error occured or a char* with the contents of the file
*/
unsigned char* download_file(char* url, int32* DNS_LIST, int32 port, uint32* out_fileSize, FILE* log);


/* this should be called once and that should happen before the frist download_file call. this function has open ssl init stuff*/
void downloader_init(void);

/* this should be called before the progamm exist, if downloader_init was called. this function has open ssl cleanup stuff**/
void donwloader_cleanup(void);



/* 

	
	INTERNAL STUFF HERE, ONLY EXPOSED FOR TESTING!


*/

/*included_downloader_h*/
#endif