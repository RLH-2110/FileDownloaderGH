#ifndef included_urlParse_h
#define included_urlParse_h

#include "../int.h"

typedef struct parsedUrl {
	char* protocol;
	char* hostname;
	char* path;
	uint16 port;
} parsedUrl;


/*
	validates the url, and if its a valid url, splits it into its components (protocol, hostname, path)

	url: the url to validate and split into its components

	returns: a pointer to a parsedUrl struct from the url (example: spliting https://pubs.opengroup.org/onlinepubs/007904975/basedefs/sys/socket.h.html into https, pubs.opengroup.org, /onlinepubs/007904975/basedefs/sys/socket.h.html
		or returns NULL if the url was not valid or an error occured.

	errno: 
		0: no error
		EINVAL: URL is NULL
		EOVERFLOW: URL is too long
		ENOMEM: can not allocate memory for the struct



	Details:
	Urls with protocols other than https are considered invalid. so http and ftp are not valid urls for this function. if you leave out the protocol, https is assumed.
	ipv6 inputs are invalid. ipv4 addresses will be seen as valid however.
	if an url has less than 3 labels, 'www.' is prependet to the hostname, but only if the first label is not allready 'www.'
	localhost will be interpreted as www.localhost
	on errors NULL is returned
	you must use punycode if you want to have urls with unicode, urls with characters not permited in the hosname are considered invalid.
	user info is NOT supported (somthing like www.abc@user.com).
	trailing dots in the hostname will be ignored i.e. www.google.com. -> www.google.com
	
	as stated before, if https is missing or you have only 2 labels with none of them being 'www', then these will be added internally in the function i.e. google.de -> https:\\www.google.de\ before it is validated and split *(the implematiotion is a bit different, but this explaination is easier)

	THE CALLER MUST FREE THE struct, NEVER free the fields of the struct, since they belong to the same memory as the struct. (areana)
*/
parsedUrl* parse_URL(char* url);



/* included_urlParse_h */
#endif