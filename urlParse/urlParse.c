#include "../int.h"
#include "../defines.h"
#include <errno.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifndef defined_parsedUrl
#define defined_parsedUrl

	typedef struct parsedUrl {
		char* protocol;
		char* hostname;
		char* path;
		uint16 port;
	} parsedUrl;

#endif


/* "Header" */


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
static parsedUrl* parse_URL(char* url);






/* Internal stuff from here on out: */

int strncmp_ignore_case(char* str1, char* str2, size_t length) {

	size_t i;

	for (i = 0; i < length; i++) {
		
		if (tolower(str1[i]) != tolower(str2[i]))
			return 1;
	}
	return 0;
}


/*
	searches for a char of an array staring at start_index.
	due to the error handleing, you can not find out if anything exist at index 0

	buff: the buffer to search
	length: the length of the buffer
	start_index: at what index the search starts
	find: the character we want to find

	return:
		0: there was an error
		other: the index where the char was found.
*/
static uint32 findInBuff(char* buff, uint32 length, uint32 start_index, char find);

/*
	searches for a string of an array staring at start_index.
	due to the error handleing, you can not find out if anything exist at index 0

	buff: the buffer to search
	length: the length of the buffer
	start_index: at what index the search starts
	find: the string we want to find

	return:
		0: there was an error
		other: the index where the char was found.
*/
static uint32 findInBuffStr(char* buff, uint32 length, uint32 start_index, char* find);

/* 
	gets the length of the string, excluding the null terminator. (like strlen)

	str: the string you want to get the length of

	returns: the length of the string

	errors:
		EINVAL: NULL was passed into str
		EOVERFLOW: the string length can not fit into a 32 bit integer
*/
static uint32 strlen32(char* str);

/* 
	converts a string to an unsigned 32 bit integer.
	this function checks if all chars are numberic, and if the value fits into a 32 bit integer.

	str: the string to convert
	len: the length of the string

	errors:
		when an errno was set, the function always returns with 0.
		EINVAL: the string contains non numberic characters or the string is NULL or the length is 0;
		EOVERFLOW: the number can not fit in an 32 bit integer

	returns: the number thats written as a string
*/
static uint16 atoui_strict(char* str, uint32 len);

/* const strings */
static char CONST_HTTPS_STRING[] = "https";
static char CONST_EMPTY_URL_PATH_STRING[] = "/";
static char CONST_WWW_LABEL_STRING[] = "www.";

static parsedUrl* parse_URL(char* url) {
	parsedUrl* ret = NULL;

	char* protocol = NULL;
	unsigned int protocol_lenght = 0;
	char* hostname = NULL;
	unsigned int hostname_lenght = 0;
	char* path = NULL;
	unsigned int path_lenght = 0;
	uint32 url_length;

	uint32 i;
	uint32 tmp;
	bool no_path = false;
	bool add_www = false;
	uint16 port = 443;
	int label_count = 0;

	errno = 0;
	url_length = strlen32(url); /* can create EINVAL if url is NULL or EOVERFLOW if its way too long*/
	if (errno != 0)
		return NULL;




	/* find protocol */
	tmp = findInBuffStr(url, url_length, 0, "://");
	if (tmp == 0) {
		protocol = CONST_HTTPS_STRING;
		protocol_lenght = strlen(CONST_HTTPS_STRING);
		i = 0; /*set i to first byte of hostname*/
	}
	else {

		if (tmp == strlen(CONST_HTTPS_STRING)) {

			if (strncmp_ignore_case(url, CONST_HTTPS_STRING, strlen(CONST_HTTPS_STRING)) == 0) {
				protocol = url + 0;
				protocol_lenght = tmp;
				i = tmp + strlen("://"); /*set i to first byte of hostname*/
			}
			else return NULL; /* the protocol is not HTTPS!*/

		}
		else return NULL; /* the protocol is not HTTPS*/
	}

	if (i >= url_length)
		return NULL;



	/* find hostname */
	if (url[i] == '/')
		return NULL; /* hostname cant start with '/' */
	hostname = url + i;
	tmp = findInBuff(url, url_length, i, '/');
	if (tmp == 0) {

		/* check for other marks that could end the hostname*/
		tmp = findInBuff(url, url_length, i, '?');
		if (tmp == 0)
			tmp = findInBuff(url, url_length, i, '#');

		if (tmp == 0)
			hostname_lenght = strlen(hostname);
		else
			hostname_lenght = tmp - i;

		no_path = true;
	}
	else {
		hostname_lenght = tmp - i;
		i = tmp; /* i is on the start of the path now*/
	}

	if (hostname[hostname_lenght - 1] == '.') /* ignore trailing dots */
		hostname_lenght--;

	if (i >= url_length)
		return NULL;


	/* path */
	if (no_path == true){
		path = CONST_EMPTY_URL_PATH_STRING;
		path_lenght = strlen(CONST_EMPTY_URL_PATH_STRING);
	}else{

		path = url + i;

		/* see if there are any chars that could end the path*/
		tmp = findInBuff(url, url_length, i, '?');
		if (tmp == 0)
			tmp = findInBuff(url, url_length, i, '#');
		
		if (tmp == 0)
			path_lenght = strlen(url + i);
		else
			path_lenght = tmp - i;
	}
	
	/* port */
	if (hostname[0] == ':')
		return NULL; /* hostname cant start with ':' */
	tmp = findInBuff(hostname,hostname_lenght,0,':');
	if (tmp != 0){
		

		errno = 0;
		port = atoui_strict(hostname+tmp+1, hostname_lenght - tmp - 1);
		if (errno != 0){
			errno = 0; /* finding an invalid url is not an error*/
			return NULL;
		}
		
		if (port == 0)
			return 0; /* port 0 is reserved */

		hostname_lenght = tmp; /* exclude the port stuff from the hostname*/
	}


	/* https://en.wikipedia.org/wiki/Hostname#Syntax */

	/* validate hostname */
	if (hostname_lenght > 253) /* max hosname length*/
		return NULL;


	/* validate hosname lenghts, and hypen rule */
	i = 0;

	while(true){

		if (hostname[i] == '.' || hostname[i] == '-')
			return NULL; /* labels cant start with a dot or hypen! */
	    tmp = findInBuff(hostname,hostname_lenght,i,'.');


	    if (tmp == 0){

	    	if (i == 0){ /* we only have one segment*/
				if (hostname_lenght > 63) 
	    			return NULL;
	    	}
	    	else /* last label, but not the first one */
	    	{
	    		if (hostname_lenght - i - 1 > 63) /* i - 1 = position of last dot*/
					return NULL;
				label_count++;
	    	}

	    	break;
	    }
		else { /* tmp != 0*/

			if (tmp - i > 63){ /* tmp - i = length of the domain/subdomain */
				return NULL;
			}
			if (hostname[tmp - 1] == '-')
				return NULL; /* labels cant end with a hypen! */
			
			label_count++;
	    }
	    i = tmp + 1;
	}

	/* if we have less than 3 labels, we might be missing the subdomain. this function will add 'www.' if there is no subdomain.*/
	if (label_count < 3) {
		if (findInBuff(hostname, hostname_lenght, 0, '.') != 3) {
			add_www = true;
			if (hostname_lenght + strlen(CONST_WWW_LABEL_STRING) > 253) /* if adding www. would bring us over the limit*/
				return NULL;
		}
		else {
			if ((hostname[0] == 'w' || hostname[0] == 'W') && (hostname[1] == 'w' || hostname[1] == 'W') && (hostname[2] == 'w' || hostname[3] == 'W')) {
				add_www = false;
			}
			else {
				add_www = true;
				if (hostname_lenght + strlen(CONST_WWW_LABEL_STRING) > 253) /* if adding www. would bring us over the limit*/
					return NULL;
			}
		}
	}



	/* validate hostname ascii characters */

	if(hostname[hostname_lenght-1] == '-')
		return NULL; /* hosnames can not end with a hypen! */

	for (i = 0; i < hostname_lenght; i++){
		if (hostname[i] >= '0' && hostname[i] <= '9')
			continue; /* 0-9 are valid */
		if (hostname[i] >= 'a' && hostname[i] <= 'z')
			continue; /* a-z is valid */
		if (hostname[i] >= 'A' && hostname[i] <= 'Z')
			continue; /* A-Z is valid */	
		if (hostname[i] == '.' || hostname[i] == '-')
			continue; /* . are seperators for labels and hypons are valid*/

		return NULL; /* found an invalid character */
	}


	/* https://stackoverflow.com/questions/4669692/valid-characters-for-directory-part-of-a-url-for-short-links */
	/* allowed chars: a-z A-Z 0-9 . - _ ~ ! $ & ' ( ) * + , ; = : @   (path seperator /,  percent encoding %00) */
	/* validate path */

	for (i = 0; i < path_lenght; i++){
		if (path[i] >= '0' && path[i] <= '9')
			continue; /* 0-9 are valid */
		if (path[i] >= 'a' && path[i] <= 'z')
			continue; /* a-z is valid */
		if (path[i] >= 'A' && path[i] <= 'Z')
			continue; /* A-Z is valid */	
		if (path[i] >= '&' && path[i] <= '/')
			continue; /* & ' ( ) * + , - . / are valid*/
		if (path[i] == '!' || path[i] == '$')
			continue; /* ! and $ are valid */
		if (path[i] == ':' || path[i] == ';')
			continue; /* : and ; are valid */
		if (path[i] == '=' || path[i] == '~')
			continue; /* = and ~ are valid */
		if (path[i] == '_' || path[i] == '@')
			continue; /* _ and @ are valid */

		if (path[i] == '%'){
			if (i + 2 < path_lenght){

				if (path[i+1] < '0' || path[i+1] > '9')
					return NULL; /* must be digit */
				if (path[i+2] < '0' || path[i+2] > '9')
					return NULL; /* must be digit */
				i += 2;
				continue;
			}else{
				return NULL; /* invalid % syntax */
			}
		}

		return NULL; /* found an invalid character */
	}


	/* the url is valid! */

	if (add_www == true) /* space for the alloc, if we need to add www.*/
		tmp = 4;
	else
		tmp = 0;

	i = 0;
	#ifdef DEBUG
		i = 1;
	#endif


#define parseUrlAlloc_len sizeof(parsedUrl) + protocol_lenght + 1 + hostname_lenght + 1 + tmp + path_lenght + 1 + i

	/* build some sort of area that contains the struct and all the stuff that belongs to it. */
	ret = malloc(parseUrlAlloc_len);
	if (ret == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	#ifdef DEBUG
	((char*)ret)[parseUrlAlloc_len-i] = 0x12;
	#endif

	ret->port = port;

	i = sizeof(parsedUrl);
	ret->protocol = ((char*)ret) + i;
	memcpy(ret->protocol, protocol,protocol_lenght);
	((char*)ret)[i+protocol_lenght] = '\0';

	i += protocol_lenght + 1;
	ret->hostname = ((char*)ret) + i;
	if (add_www) {
		memcpy(ret->hostname, CONST_WWW_LABEL_STRING, strlen(CONST_WWW_LABEL_STRING)); /* copy in www. if we need to add it*/
	}
	memcpy(ret->hostname + tmp, hostname, hostname_lenght);
	((char*)ret)[i+tmp+hostname_lenght] = '\0';

	i += hostname_lenght + tmp + 1;
	ret->path = ((char*)ret) + i;
	memcpy(ret->path, path, path_lenght);
	((char*)ret)[i+path_lenght] = '\0';

	#ifdef DEBUG
		if (((char*)ret)[parseUrlAlloc_len-i] != 0X12) {
			puts("DEBUG: OOPS: memory corruption in parse_URL");
			errno = ENOMEM;
			free(ret);
			return NULL;
		}
	#endif

	return ret;

#undef parseUrlAlloc_len
}

static uint32 strlen32(char* start){
	char* curr = start;

	if (curr == NULL){
		errno = EINVAL;
		return 0;
	}

	while(*curr != '\0')
		curr++;
	
	if (curr - start > 0xFFFFFFFF){ /* biger than uint32?*/
		errno = EOVERFLOW;
		return 0;
	}
	return curr - start;
}

static uint32 findInBuff(char* buff, uint32 length, uint32 start_index, char find){
	uint32 i = start_index;

	while(true){
		if (i >= length)
			return 0;

		if (buff[i] == find)
			return i;

		i++;
	}

}



static uint32 findInBuffStr(char* buff, uint32 length, uint32 start_index, char* find){
	uint32 strI;
	uint32 hit;
	uint32 buffI = start_index;

	while(true){
		if (buffI >= length)
			return 0;

		if (buff[buffI] == find[0]){
			hit = buffI;

			buffI++;
			strI = 1;
			while(strI < strlen(find)){

				if (buffI >= length)
					return 0;

				if (buff[buffI] != find[strI]){
					break;
				}
				strI++;
				buffI++;
			}

			if (strI == strlen(find))
				return hit;
		}

		buffI++;
	}

}



static uint16 atoui_strict(char* str, uint32 len){
	uint16 val = 0;
	unsigned int i; 

	if (str == NULL || len == 0){
		errno = EINVAL;
		return 0;
	}

	for(i = 0; i < len; i++){
		if (str[i] < '0' || str[i] > '9'){
			errno = EINVAL;
			return 0;
		}


		/* overflow detection */
		{
			if (val > 0xFFFF / 10){ /* check if mul by 10 would overflow*/
				errno = EOVERFLOW;
				return 0;
			}
			if (val * 10 > 0xFFFF - (str[i] - '0')) { /* check if adding (str[i] - '0') after that would overflow */
				errno = EOVERFLOW;
				return 0;
			}
		}

		val *= 10; /* "shift" the difits left, to make space for the new one*/
		val += str[i] - '0';
	}
	return val;
}

#undef DEBUG