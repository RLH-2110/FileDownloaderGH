
#ifdef POSIX
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <errno.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "int.h"

#include "downloader_internal.h"
#include "downloader.h"
#include "DNS_offsets.h"
#include "defines.h"


/*  https://pubs.opengroup.org/onlinepubs/007904975/basedefs/sys/socket.h.html */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>


void downloader_init(void) {
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
}

void donwloader_cleanup(void) {
	EVP_cleanup();
}


int32 DNS_lookup(char* url, int32* DNS_LIST, FILE* log){

	char* tmp;
	uint32 ip;
	struct timeval tv;
	struct sockaddr_in server_addr;
	int ret;
	unsigned char* DNS_request;
	parsedUrl* p_url;
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
		DNS_request = generate_DNS_request(p_url->hostname, id, &request_size, log);
		free(p_url); p_url = NULL;
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

unsigned char* download_file(char* url, int32* DNS_LIST, int32 port, uint32* out_fileSize, FILE* log){
	parsedUrl* p_url;
	int32 ipv4;
	struct timeval tv;
	struct sockaddr_in server_addr;
	int ret;
	int sock;
	unsigned char* buff;
	unsigned char* oldbuff;
	int32 bufflen;
	int32 total_writen;
	size_t bytes_read;
	SSL_CTX* ctx;
	SSL* ssl;
	const SSL_METHOD* method = TLS_client_method();
	uint32 i;
	uint32 content_start_index;
	uint8 matches;

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
	

	if (p_url->rest == NULL || p_url->hostname == NULL){
		putslog("URL Parseing error");
		SSL_CTX_free(ctx);
		free(p_url); p_url = NULL;
		return NULL;
	}

	bufflen = strlen(DOWNLOAD_GET_REQUEST) + strlen(p_url->hostname) + strlen(p_url->rest) + 1;
	buff = malloc(bufflen);
	if (buff == NULL){
		free(p_url); p_url = NULL;
		SSL_CTX_free(ctx);
		putslog("Out of memory!");
		return NULL;
	}

	sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); 
	if (sock < 0) {
    	perrorlog("socket failed");
    	SSL_CTX_free(ctx);
		free(p_url); p_url = NULL;
    	return NULL;
	}

	/* setting socket options */
	tv.tv_sec = 0;
	tv.tv_usec = 500000; /* this is in microseconds and equals to 500 ms) */
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) != 0){
		perrorlog("setting socket timeout failed");
		free(p_url); p_url = NULL;
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
		free(p_url); p_url = NULL;;
		return NULL;
	}

	sprintf((char*)buff,DOWNLOAD_GET_REQUEST,p_url->rest,p_url->hostname); /* generate the get request */
	free(p_url); p_url = NULL;



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

	bufflen = 4096;
	buff = malloc(bufflen);
	if (buff == NULL) {
		putslog("Out of memory");
		SSL_free(ssl);
		close(sock);
		SSL_CTX_free(ctx);
		return NULL;
	}

	i = 0;
	total_writen = 0;
	matches = 0;
	while (true) {

		/* get http header */
		if (SSL_read_ex(ssl, buff + total_writen, bufflen - total_writen, &bytes_read) != 1) {
			putslog("some ssl error happened");

		download_file_cleanup_1:
			SSL_free(ssl);
			close(sock);
			SSL_CTX_free(ctx);
			return NULL;
		}

		if (bytes_read == 0) {
			putslog("header not found");
			goto download_file_cleanup_1;
		}

		total_writen += bytes_read;
		if (log)
			fprintf(log, "%d bytes read. while searching for the header\n", total_writen);

		for (;i < total_writen;i++) {
			if ((matches & 1) == 0) {
				if (buff[i] == '\r')
					matches++;
				else
					matches = 0;
			}
			else {
				if (buff[i] == '\n')
					matches++;
				else
					matches = 0;
			}

			if (matches >= 4)
				break;
		}

		if (matches >= 4) /* if we found the end of the header*/
			break;
	}

	content_start_index = i + 1;


	bufflen = httpResponseGetContentSize(buff, bytes_read, log);
	printf("allocating space for %d elements\n", bufflen);


	oldbuff = buff;
	buff = malloc(bufflen);
	if (buff == NULL) {
		putslog("Out of memory");
		SSL_free(ssl);
		close(sock);
		SSL_CTX_free(ctx);
		return NULL;
		}


	/* copy over existing context, if we have any*/
	if (content_start_index < total_writen)
		memcpy(buff, oldbuff + content_start_index, total_writen - content_start_index);
	free(oldbuff); oldbuff = NULL;

	total_writen = total_writen - content_start_index;

	while (bufflen - total_writen > 0) {
		if (SSL_read_ex(ssl, buff + total_writen, bufflen - total_writen, &bytes_read) != 1) {
			putslog("some ssl error happened");
			break;
		}

		if (bytes_read == 0)
			break;

		if (bufflen < total_writen + bytes_read) {
			putslog("overflow avoided!");
			break;
		}

		total_writen += bytes_read;
		if (log)
			fprintf(log, "%d bytes read. %d left.\n", total_writen, bufflen - total_writen);
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

	*out_fileSize = bufflen;

    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);

	return buff;
}

/* POSIX */
#endif

typedef int make_iso_compiler_happy;