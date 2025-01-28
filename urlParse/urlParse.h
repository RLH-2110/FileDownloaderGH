#ifndef included_urlParse_h
#define included_urlParse_h

#include "../int.h"

typedef struct parsedUrl {
	char* protocol;
	char* hostname;
	char* path;
	uint32 port;
} parsedUrl;


/*
	validates the url, and if its a valid url, splits it into its components (protocol, hostname, path)

	url: the url to validate and split into its components

	returns: a pointer to a parsedUrl struct from the url (example: spliting https://pubs.opengroup.org/onlinepubs/007904975/basedefs/sys/socket.h.html into https, pubs.opengroup.org, /onlinepubs/007904975/basedefs/sys/socket.h.html
		or returns NULL if the url was not valid or an error occured.

	errno: 
		(unchanged) = no error
		... (TODO)


	Details:
	Urls with protocols other than https are considered invalid. so http and ftp are not valid urls for this function. if you leave out the protocol, https is assumed.
	ipv4 and ipv6 inputs are also invalid.
	localhost will be interpreted as www.localhost

	if you do not put in stuff like https, or www. then they will be added. i.e. google.de -> https:\\google.de\ before it is validated and split *(the implematiotion is a bit different, but this explaination is easier)

	THE CALLER MUST FREE THE struct, NEVER free the fields of the struct, since they belong to the same memory as the struct. (areana)
*/
parsedUrl* parse_URL(char* url);



/* included_urlParse_h */
#endif