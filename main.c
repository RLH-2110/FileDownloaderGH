#ifndef NULL
#define NULL (void*)0
#endif


#ifdef POSIX

/*  https://pubs.opengroup.org/onlinepubs/007904975/basedefs/sys/socket.h.html */
#include <sys/socket.h>


char* download_file(char* url){

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

	
	int sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); 
	if (sock < 0) {
    	perror("socket failed");
    	return NULL;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(53); /* DNS port */
	server_addr.sin_addr.s_addr = htonl(DNS_LIST[0]); 

	ret = connect(sock,&server_addr,sizeof(struct sockaddr_in));
	if (ret != 0){
		perror("connect failed");
		return NULL;
	}

	// read this and then continue
	// https://mislove.org/teaching/cs4700/spring11/handouts/project1-primer.pdf

	return "hello world";
}

/* POSIX */
#endif


#ifdef WINDOWS

#endif

