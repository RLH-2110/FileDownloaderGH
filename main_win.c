#ifdef WINDOWS
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>

	#include <time.h>
	#include <errno.h>

	#include "int.h"

	#include "downloader.h"
	#include "DNS_offsets.h"
	#include "defines.h"


	#include <winsock2.h>
	#include <windows.h>
		
	int32 DNS_lookup(char* url, int32* DNS_LIST, FILE* log){

		char* tmp;
		uint32 ip;
		DWORD tv;
		struct sockaddr_in  server_addr;
		int ret;
		unsigned char* DNS_request;
		parsedUrl p_url;
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
			DNS_request = generate_DNS_request(p_url.hostname, id, &request_size, log);
			free(p_url.hostname);		free(p_url.protocol);		free(p_url.rest); p_url.hostname = NULL; p_url.protocol = NULL; p_url.rest = NULL;


			putslog("debug: Trying send...");

			if (sendto(sock, DNS_request, request_size, 0, (struct sockaddr*)&server_addr, address_len) == -1) {
				perrorlog_winsock("error while doing sentto");
				closesocket(sock);
				continue;
			}
			free(DNS_request);
				

			putslog("debug: Trying to recive...");

			errno = 0;

			#define recv_len 1024
			DNS_request = malloc(recv_len); /* 1 KiB ought to be enough for everyone*/
			if (recvfrom(sock, DNS_request, recv_len, 0, (struct sockaddr*)&server_addr, &address_len) == -1) {

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

	char* download_file(char* url, int32* DNS_LIST, int32 port, uint32* out_fileSize, FILE* log){
		puts("Not implemented");
		return NULL;
	}


#endif