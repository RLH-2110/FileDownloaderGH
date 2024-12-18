#include <stdlib.h>
#include <string.h>

#ifndef NULL
#define NULL (void*)0
#endif


#ifdef POSIX

/*  https://pubs.opengroup.org/onlinepubs/007904975/basedefs/sys/socket.h.html */
#include <sys/socket.h>


enum trimState{
	TS_findDot,
	TS_trimStart,
	TS_findEnd,
	TS_fixSubdomain,
	TS_exit
};

/* 
	url: the url to normalize

	returns a normalized url from the url (example: turning https://pubs.opengroup.org/onlinepubs/007904975/basedefs/sys/socket.h.html to pubs.opengroup.org
	this is just a char* srting or NULL if there was an error

	THE CALLER MUST FREE THE NORMALIZED URL!
*/
char* trim_URL(char* url){

	char* normalized_url = NULL;

	int dotIndex, currentIndex, endIndex, startIndex;	
	int dots = 0;
	int state = TS_exit;
	startIndex = 0;

	while(state != TS_exit){

		switch (state){


			case TS_findDot:
			{
				if (url[currentIndex] == 0)
					return NULL;

				if (url[currentIndex] != '.'){
					currentIndex++;
					break;
				}
				/* found a . */
				dots++;
				dotIndex = currentIndex;
				state = TS_trimStart;
				currentIndex--;
				break;
			}



			case TS_trimStart:
			{
				if (currentIndex < 0)
					return NULL;

				if (currentIndex == 0)
					goto TS_trimStart_nextSate;

				if (url[currentIndex] == '/'){
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

				if (url[currentIndex] == '.'){
					dots++;
					if (dots > 2)
						return NULL; /* weird url */
				}

				if (url[currentIndex] == '/'){
					currentIndex--;
TS_findEnd_nextSate:
					endIndex = currentIndex;
					state = TS_fixSubdomain;
					currentIndex = dotIndex;
					break;
				}

				currentIndex++;
				break;
			}

			case TS_fixSubdomain:
			{
				/* +2 because index is always one smaller, and we need a null byte*/
				#define normalizedMallocSsize endIndex + 2 - startIndex 
				if (dots == 2){

					normalized_url = malloc(normalizedMallocSsize); 
					memcpy(normalized_url,url+startIndex,normalizedMallocSsize);
					normalized_url[normalizedMallocSsize-1] = 0;

					state = TS_exit;
					break;

				}else{

					normalized_url = malloc(normalizedMallocSsize + 3); /* +4 because we need to add www. */
					memcpy(normalized_url,url+startIndex+4,normalizedMallocSsize); /* +4 because www. is missing*/
					normalized_url[0] = 'w';
					normalized_url[1] = 'w';
					normalized_url[2] = 'w';
					normalized_url[3] = '.';
					normalized_url[normalizedMallocSsize+4-1] = 0;

					state = TS_exit;
					break;

				}
			}

		

		}


	}

	return normalized_url;

}


char* generate_DNS_restuest(char* normalized_url){
	return NULL;
}

char* download_file(char* url){
/*
	struct sockaddr_in server_addr;
	int ret;

	if (url == NULL || DNS_LIST == NULL){
		perror("url and DNS_LIST must not be zero!");
		return NULL;
	}

	if (DNS_LIST[0] == 0){
		perror("DNS_LIST is emptry")
		return NULL;
	}


	/* UNTESTED CODE STARTS HERE*/

	
/*	int sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); 
	if (sock < 0) {
    	perror("socket failed");
    	return NULL;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(53); /* DNS port */
/*	server_addr.sin_addr.s_addr = htonl(DNS_LIST[0]); 

	ret = connect(sock,&server_addr,sizeof(struct sockaddr_in));
	if (ret != 0){
		perror("connect failed");
		return NULL;
	}

	/* read this and then continue
	/ https://mislove.org/teaching/cs4700/spring11/handouts/project1-primer.pdf
	*/

	return "hello world";
}

/* POSIX */
#endif


#ifdef WINDOWS

#endif

