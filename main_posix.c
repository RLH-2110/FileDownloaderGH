#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <errno.h>

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

char* download_file(char* url, int32* DNS_LIST, FILE* log){
	puts("Not implemented");
	return NULL;
}

/* POSIX */
#endif

