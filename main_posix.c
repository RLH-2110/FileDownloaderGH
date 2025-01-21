#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <errno.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "int.h"

#include "downloader.h"
#include "DNS_offsets.h"
#include "defines.h"


#ifdef POSIX

/*  https://pubs.opengroup.org/onlinepubs/007904975/basedefs/sys/socket.h.html */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>



int32 DNS_lookup(char* url, int32* DNS_LIST, FILE* log){

	char* tmp;
	uint32 ip;
	struct timeval tv;
	struct sockaddr_in server_addr;
	int ret;
	char* DNS_request;
	parsedUrl p_url;
	uint16 id;
	int request_size;
	int sock;
	int DNSindex; /* the DNS_LIST index*/
	socklen_t address_len = sizeof(struct sockaddr_in);

	if (url == NULL || DNS_LIST == NULL){
		perrorlog("url and DNS_LIST must not be zero!");
		return 0;
	}

	if (DNS_LIST[0] == 0){
		perrorlog("DNS_LIST is emptry");
		return 0;
	}

	if (url[0] == '\0') {
		putslog("gave an empty url!");
		return 0;
	}



	for(DNSindex = 0;;DNSindex++){
		if (DNS_LIST[DNSindex] == 0){
			putslog("Can not connect to any dns in the list!");
			return 0;
		}

		
		sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); 
		if (sock < 0) {
	    	perrorlog("socket failed");
	    	return 0;
		}

		/* setting socket options */
		tv.tv_sec = 0;
		tv.tv_usec = 500000; /* this is in microseconds and equals to 500 ms) */
		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) != 0){
			perrorlog("setting socket timeout failed");
			close(sock);
			return 0;
		}

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(53); /* DNS port */
		server_addr.sin_addr.s_addr = htonl(DNS_LIST[DNSindex]); 

		tmp = IPv4ToStringR(htonl(DNS_LIST[DNSindex]));
		printflog("trying DNS: %s\n", tmp);
		free(tmp);

		putslog("debug: Trying to connect...");

		ret = connect(sock, (struct sockaddr*)&server_addr, address_len);
		if (ret != 0){
			perrorlog("connect failed");
			close(sock);
			continue;
		}

		id = (uint16)time(NULL);
		p_url = parse_URL(url);
		DNS_request = generate_DNS_request(p_url.hostname, id, &request_size, log);
		free(p_url.hostname);		free(p_url.protocol);		free(p_url.rest); p_url.hostname = NULL; p_url.protocol = NULL; p_url.rest = NULL;
		if (DNS_request == NULL){
			putslog("URL Parseing error");
			return 0;
		}

		putslog("debug: Trying send...");

		if (sendto(sock, DNS_request, request_size, MSG_EOR, (struct sockaddr*)&server_addr, address_len) == -1) {
			perrorlog("error while doing sentto");
			free(DNS_request);
			close(sock);
			return 0;
		}
		free(DNS_request);
			

		putslog("debug: Trying to recive...");

		errno = 0;

		#define recv_len 1024
		DNS_request = malloc(recv_len); /* 1 KiB ought to be enough for everyone*/
		if (recvfrom(sock, DNS_request, recv_len, MSG_WAITALL, (struct sockaddr*)&server_addr, &address_len) == -1) {

			if (errno == EAGAIN || errno == EWOULDBLOCK){ /* Timeout */
			}else{
				perrorlog("error while doing recvfrom")
			}
			free(DNS_request);;
			close(sock);
			continue;
		}

		putslog("got a response!");


		ip = DNS_parse_reply(DNS_request, id, recv_len, log);
		free(DNS_request);
		close(sock);

		if (ip == 0)
			continue;
		
		return ip;
	}

	return 0;

}

const char* DOWNLOAD_GET_REQUEST = "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "\r\n";

char* download_file(char* url, int32* DNS_LIST, int32 port, uint32* out_fileSize, FILE* log){
	parsedUrl p_url;
	int32 ipv4;
	struct timeval tv;
	struct sockaddr_in server_addr;
	int ret;
	int sock;
	char* buff;
	char* oldbuff;
	int32 bufflen;
	int32 total_writen;
	size_t bytes_read;
	SSL_CTX* ctx;
	SSL* ssl;
	const SSL_METHOD* method = TLS_client_method();

	putslog("calling download_file");

	if (out_fileSize == NULL){
		putslog("out_fileSize can not be NULL!");
		return NULL;
	}

	if (url == NULL || DNS_LIST == NULL){
		perrorlog("url and DNS_LIST must not be zero!");
		return NULL;
	}

	if (DNS_LIST[0] == 0){
		perrorlog("DNS_LIST is emptry");
		return NULL;
	}

	if (url[0] == '\0') {
		putslog("gave an empty url!");
		return NULL;
	}


    ctx = SSL_CTX_new(method);

    if (!ctx) {
        perrorlog("Unable to create SSL context");
        return NULL;
    }


	ipv4 = DNS_lookup(url, DNS_LIST, log);

	/*puts("using debug ip instead...");
	ipv4 = 0xB9C76E85;*/

	p_url = parse_URL(url);
	

	if (p_url.rest == NULL || p_url.hostname == NULL){
		putslog("URL Parseing error");
		SSL_CTX_free(ctx);
		return NULL;
	}

	bufflen = strlen(DOWNLOAD_GET_REQUEST) + strlen(p_url.hostname) + strlen(p_url.rest) + 1;
	buff = malloc(bufflen);
	if (buff == NULL){
		free(p_url.hostname);		free(p_url.protocol);		free(p_url.rest);p_url.hostname = NULL; p_url.protocol = NULL; p_url.rest = NULL;
		SSL_CTX_free(ctx);
		putslog("Out of memory!");
		return NULL;
	}

	sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); 
	if (sock < 0) {
    	perrorlog("socket failed");
    	SSL_CTX_free(ctx);
    	free(p_url.hostname);		free(p_url.protocol);		free(p_url.rest); p_url.hostname = NULL; p_url.protocol = NULL; p_url.rest = NULL;
    	return NULL;
	}

	/* setting socket options */
	tv.tv_sec = 0;
	tv.tv_usec = 500000; /* this is in microseconds and equals to 500 ms) */
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) != 0){
		perrorlog("setting socket timeout failed");
		free(p_url.hostname);		free(p_url.protocol);		free(p_url.rest); p_url.hostname = NULL; p_url.protocol = NULL; p_url.rest = NULL;
		SSL_CTX_free(ctx);
		close(sock);
		return NULL;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port); 
	server_addr.sin_addr.s_addr = htonl(ipv4); 


	putslog("trying to connect...");

	ret = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (ret != 0){
		perrorlog("connect failed");
		SSL_CTX_free(ctx);
		close(sock);
		free(p_url.hostname);		free(p_url.protocol);		free(p_url.rest); p_url.hostname = NULL; p_url.protocol = NULL; p_url.rest = NULL;
		return NULL;
	}

	sprintf(buff,DOWNLOAD_GET_REQUEST,p_url.rest,p_url.hostname); /* generate the get request */
	free(p_url.hostname);		free(p_url.protocol);		free(p_url.rest); p_url.hostname = NULL; p_url.protocol = NULL; p_url.rest = NULL;



	putslog("trying setup ssl...");
	/* Set up SSL */
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
    	if (log != NULL)
        	ERR_print_errors_fp(log);
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        return NULL;
    }



	putslog("trying to send...");

    if (SSL_write(ssl, buff, bufflen) <= 0) {
     	perrorlog("error while doing SSL_write");

     	free(buff); buff = NULL; bufflen = 0;
     	if (log != NULL)
        	ERR_print_errors_fp(log);
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        return NULL;
    }

	free(buff); buff = NULL; bufflen = 0;

	

	putslog("trying to receive...");

    bufflen = 4096 * 5;
    bufflen = 1048576;
	buff = malloc(bufflen);
	if (buff == NULL){
		putslog("Out of memory");
		close(sock);
		return NULL;
	}



	total_writen = 0;

    while (SSL_read_ex(ssl, buff + total_writen, bufflen - total_writen, &bytes_read) == 1) {

    	if(bytes_read == 0)
    		break;

    	if(bufflen < total_writen + bytes_read){
    		total_writen += bytes_read;
    		break;
    	}

        total_writen += bytes_read;
    }

    if (total_writen <= 0) {
        if (log != NULL)
        	ERR_print_errors_fp(stderr);
        free(buff); bufflen = 0; buff = NULL;
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        return NULL;
    }


 /* Read response */
/*
    while (SSL_read_ex(ssl, buff, bufflen - total_writen, &bytes_read) == 1) {

    	if(bytes_read == 0)
    		break;

    	if(bufflen < total_writen + bytes_read){
    		total_writen += bytes_read;
    		break;
    	}

    	/* moar space*/

/*    	oldbuff = buff;
    	bufflen = bufflen * 1.3; /* increase buff by 30%*/
/*    	buff = realloc(buff,bufflen);

    	if (buff == NULL)
    	{
    		free(oldbuff); bufflen = 0; oldbuff = NULL; buff = NULL;
    		 SSL_free(ssl);
		    close(sock);
		    SSL_CTX_free(ctx);
    		putslog("Out of memory");
    		return NULL;
    	}
    	oldbuff = NULL;

        total_writen += bytes_read;
    }

    if (total_writen <= 0) {
        if (log != NULL)
        	ERR_print_errors_fp(stderr);
        free(buff); bufflen = 0; buff = NULL;
        SSL_free(ssl);
        close(sock);
        SSL_CTX_free(ctx);
        return NULL;
    }
*/
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);

    oldbuff = buff;
    buff = httpResponseToRaw(buff, total_writen, out_fileSize, log);
    free(oldbuff);

	return buff;
}

/* POSIX */
#endif

