#ifdef WINDOWS
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>

	#include <time.h>
	#include <errno.h>

	#include "int.h"

	#include "downloader_internal.h"
	#include "downloader.h"
	#include "DNS_offsets.h"
	#include "defines.h"


	#include <openssl/ssl.h>
	#include <openssl/err.h>


	#include <winsock2.h>
	#include <windows.h>
		
	int32 DNS_lookup(char* url, int32* DNS_LIST, FILE* log){

		char* tmp;
		uint32 ip;
		DWORD tv;
		struct sockaddr_in  server_addr;
		int ret;
		unsigned char* DNS_request;
		parsedUrl* p_url;
		uint16 id;
		int request_size;
		int sock;
		int DNSindex; /* the DNS_LIST index*/
		WSADATA wsaData;
		int address_len = sizeof(server_addr);

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

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		    perrorlog("WSAStartup failed");
		    return 0;
		}


		for(DNSindex = 0;;DNSindex++){

			if (DNS_LIST[DNSindex] == 0){ /* terminator found */
				putslog("Can not connect to any dns in the list!");
				WSACleanup();
				return 0;
			}

			sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); 
			if (sock == INVALID_SOCKET) {
		    	perrorlog_winsock("socket failed");
		    	WSACleanup();
		    	return 0;
			}

			tv = 500; /* milliseconds */
			/* set socket options*/
			if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(DWORD)) != 0) { /* try to set timeout to 500 ms*/
				perrorlog_winsock("setting socket timeout failed");
				closesocket(sock);
				WSACleanup();
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
				perrorlog_winsock("connect failed");
				closesocket(sock);
				continue;
			}

			id = (uint16)time(NULL);
			p_url = parse_URL(url);
			DNS_request = generate_DNS_request(p_url->hostname, id, &request_size, log);
			free(p_url); p_url = NULL;


			putslog("debug: Trying send...");

			if (sendto(sock, (char*)DNS_request, request_size, 0, (struct sockaddr*)&server_addr, address_len) == -1) {
				perrorlog_winsock("error while doing sentto");
				closesocket(sock);
				continue;
			}
			free(DNS_request);
				

			putslog("debug: Trying to recive...");

			errno = 0;

			#define recv_len 1024
			DNS_request = malloc(recv_len); /* 1 KiB ought to be enough for everyone*/
			if (recvfrom(sock, (char*)DNS_request, recv_len, 0, (struct sockaddr*)&server_addr, &address_len) == -1) {

				if (WSAGetLastError() != WSAETIMEDOUT){ /* not a Timeout */
					perrorlog_winsock("error while doing recvfrom");
				}
								
				free(DNS_request); 
				closesocket(sock);
				continue;
			}

			putslog("got a response!");


			ip = DNS_parse_reply(DNS_request, id, recv_len, log);

			free(DNS_request); 
			closesocket(sock);

			if (ip != 0){
				WSACleanup();
				return ip;
			}
		}

		WSACleanup();
		return 0;
	}


    void downloader_init(void) {
    }

    void donwloader_cleanup(void) {
    }

	const char* DOWNLOAD_GET_REQUEST = "GET %s HTTP/1.1\r\n"
		"Host: %s\r\n"
		"Connection: close\r\n"
		"\r\n";

	unsigned char* download_file(char* url, int32* DNS_LIST, int32 port, uint32* out_fileSize, FILE* log) {
		parsedUrl* p_url;
		int32 ipv4;
		DWORD tv;
		struct sockaddr_in server_addr;
		int ret;
		int sock;
		unsigned char* buff;
		int32 bufflen;
		int32 total_writen;
		size_t bytes_read;
		SSL_CTX* ctx;
		SSL* ssl;
		const SSL_METHOD* method = TLS_client_method();
		WSADATA wsaData;

		putslog("calling download_file");

		if (out_fileSize == NULL) {
			putslog("out_fileSize can not be NULL!");
			return NULL;
		}

		if (url == NULL || DNS_LIST == NULL) {
			perrorlog("url and DNS_LIST must not be zero!");
			return NULL;
		}

		if (DNS_LIST[0] == 0) {
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

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			perrorlog("WSAStartup failed");
			return 0;
		}

		ipv4 = DNS_lookup(url, DNS_LIST, log);


		p_url = parse_URL(url);


		if (p_url->rest == NULL || p_url->hostname == NULL) {
			putslog("URL Parseing error");
			SSL_CTX_free(ctx);
			free(p_url); p_url = NULL;
			WSACleanup();
			return NULL;
		}

		bufflen = strlen(DOWNLOAD_GET_REQUEST) + strlen(p_url->hostname) + strlen(p_url->rest) + 1;
		buff = malloc(bufflen);
		if (buff == NULL) {
			free(p_url); p_url = NULL;
			SSL_CTX_free(ctx);
			putslog("Out of memory!");
			WSACleanup();
			return NULL;
		}

		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock < 0) {
			perrorlog("socket failed");
			SSL_CTX_free(ctx);
			free(p_url); p_url = NULL;
			WSACleanup();
			return NULL;
		}


		
		tv = 500; /* milliseconds */
		/* set socket options*/
		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(DWORD)) != 0) { /* try to set timeout to 500 ms*/
			perrorlog("setting socket timeout failed");
			free(p_url); p_url = NULL;
			SSL_CTX_free(ctx);
			closesocket(sock);
			WSACleanup();
			return NULL;
		}

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(port);
		server_addr.sin_addr.s_addr = htonl(ipv4);


		putslog("trying to connect...");

		ret = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
		if (ret != 0) {
			perrorlog("connect failed");
			SSL_CTX_free(ctx);
			closesocket(sock);
			WSACleanup();;
			free(p_url); p_url = NULL;;
			return NULL;
		}

		sprintf((char*)buff, DOWNLOAD_GET_REQUEST, p_url->rest, p_url->hostname); /* generate the get request */
		free(p_url); p_url = NULL;



		putslog("trying setup ssl...");
		/* Set up SSL */
		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, sock);

		if (SSL_connect(ssl) <= 0) {
			if (log != NULL)
				ERR_print_errors_fp(log);
			SSL_free(ssl);
			closesocket(sock);
			WSACleanup();
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
			closesocket(sock);
			WSACleanup();
			SSL_CTX_free(ctx);
			return NULL;
		}

		free(buff); buff = NULL; bufflen = 0;



		putslog("trying to receive...");

		bufflen = 1024;
		buff = malloc(bufflen);
		if (buff == NULL) {
			putslog("Out of memory");
			SSL_free(ssl);
			closesocket(sock);
			WSACleanup();
			SSL_CTX_free(ctx);
			return NULL;
		}





		/* get http header */
		SSL_read_ex(ssl, buff, bufflen, &bytes_read);

		bufflen = httpResponseGetContentSize(buff, bytes_read, log);
		printf("allocating space for %d elements\n", bufflen);

		free(buff); buff = NULL;
		buff = malloc(bufflen);
		if (buff == NULL) {
			putslog("Out of memory");
			SSL_free(ssl);
			closesocket(sock);
			WSACleanup();
			SSL_CTX_free(ctx);
			return NULL;
		}

		total_writen = 0;

		while (bufflen - total_writen > 0) {
			if (SSL_read_ex(ssl, buff + total_writen, bufflen - total_writen, &bytes_read) != 1) {
				puts("some ssl error happened");
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
			closesocket(sock);
			WSACleanup();
			SSL_CTX_free(ctx);
			return NULL;
		}

		*out_fileSize = bufflen;

		SSL_free(ssl);
		closesocket(sock);
		WSACleanup();
		SSL_CTX_free(ctx);

		return buff;
	}


#endif

typedef int make_iso_compiler_happy;