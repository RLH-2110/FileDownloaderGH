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

enum DNS_lookup_state {
	DLS_skipping,
	DLS_found
};

int32 DNS_lookup(char* url, int32* DNS_LIST, FILE* log){

	uint16 dns_answers;
	int dns_answers_index;
	int i;
	uint16 flags;
	int state;
	uint16 rdlength;
	uint32 ip_address;
	uint16 dns_questions;

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
	DNSindex = 0;



	if (!true){ /* never go here normally */

retry_with_new_DNS:
		close(sock); /* close socket */
		DNSindex++;
		if (DNS_LIST[DNSindex] == 0){
			putslog("Can not connect to any dns in the list!");
			return 0;
		}
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

	putslog("debug: Trying to connect...");

	ret = connect(sock, (struct sockaddr*)&server_addr, address_len);
	if (ret != 0){
		perrorlog("connect failed");
		goto retry_with_new_DNS;
	}

	id = (uint16)time(NULL);
	p_url = parse_URL(url);
	DNS_request = generate_DNS_request(p_url.hostname, id, &request_size, log);


	putslog("debug: Trying send...");

	if (sendto(sock, DNS_request, request_size, MSG_EOR, (struct sockaddr*)&server_addr, address_len) == -1) {
		perrorlog("error while doing sentto");
		goto DNS_lookup_cleanup;
	}
	free(DNS_request);
		

	putslog("debug: Trying to recive...");

	errno = 0;

#define recv_len 1024
	DNS_request = malloc(recv_len); /* 1 KiB ought to be enough for everyone*/
	if (recvfrom(sock, DNS_request, recv_len, MSG_WAITALL, (struct sockaddr*)&server_addr, &address_len) == -1) {

		if (errno == EAGAIN || errno == EWOULDBLOCK){ /* Timeout */
			free(DNS_request);
			goto retry_with_new_DNS;
		}

		perrorlog("error while doing recvfrom");
		goto DNS_lookup_cleanup;
	}

	putslog("got a response!");

	close(sock);







	/*   /###############\   */
	/*  /                 \  */
	/* | we got the reply! | */
	/*  \                 /  */
	/*   \###############/   */


	flags = DNS_construct_flags(DNS_request[DNS_HEADER_FLAGS_OFFSET],DNS_request[DNS_HEADER_FLAGS_OFFSET+1]);

	/* check if the ID is the same */
	if (DNS_request[DNS_HEADER_ID_OFFSET] != (char)((id & 0xFF00)>>8) || DNS_request[DNS_HEADER_ID_OFFSET+1] != (char)(id & 0xFF)){

		printf("\nresponse upper:\t %d\n excpected upper:\n %d\n\nresponse lower:\t %d\nexpected lower:\t %d\n",DNS_request[DNS_HEADER_ID_OFFSET],((id & 0xFF00)>>8),DNS_request[DNS_HEADER_ID_OFFSET+1],(id & 0xFF));

		putslog("The IDs not match!");
		goto retry_with_new_DNS;
	}
	
	/* check if its actually an answer*/
	if (DNS_FLAGS_GET_QR(flags) != 1){
		putslog("The DNS responed with a question!");
		goto retry_with_new_DNS;
	}

	/* check if there was no error */
	if(DNS_FLAGS_GET_RCODE(flags) != 0){
		switch(DNS_FLAGS_GET_RCODE(flags)){
		case 1:
			putslog("CRITICAL! DNS RESPONEDED WITH 'FORMAT ERROR' IN THE RCODE!"); break;

		case 2:
			putslog("DNS responded with 'Server failure' in RCODE"); break;
		
		case 3:
			putslog("DNS responed with 'Name Error' in RCODE"); break;
		
		case 4:
			putslog("DNS responed with 'Not Implemented' in RCODE"); break;
		
		case 5:
			putslog("DNS responed with 'Refused' in RCODE"); break;
			
		default:
			printflog("DNS responed with unrecognized RCODE: %d\n",DNS_FLAGS_GET_RCODE(DNS_request[DNS_HEADER_FLAGS_OFFSET])); break;
		}

		goto retry_with_new_DNS;
	}

	/* check that its not trunkated */
	if(DNS_FLAGS_GET_TC(flags) != 0){
		putslog("The DNS responed with tuncation!");
		goto retry_with_new_DNS;
	}

	dns_answers = DNS_request[DNS_HEADER_ANCOUNT_OFFSET] << 8;
	dns_answers |= DNS_request[DNS_HEADER_ANCOUNT_OFFSET+1];
	
	printflog("DNS has %d answers\n",dns_answers);

	puts("respone: ");
	for (i = 0; i < 1024; i++)
	{
		printf("%02X ", (unsigned char)DNS_request[i]);
	}
	printf("\n");

	i = DNS_HEADER_SIZE; /* this should now point at the first byte of the DNS response */



	/* handle if the server returns dns questions */
	dns_questions = DNS_request[DNS_HEADER_QDCOUNT_OFFSET+1] + (DNS_request[DNS_HEADER_QDCOUNT_OFFSET] << 8);

	for(;dns_questions > 0;dns_questions--){
		/* skip though the name */
		/* note: the url we asked for is stored in p_url.hostname. if we want to, we can check if the answer we got matches what we asked for*/
		while(DNS_request[i] != 0){
			
			if (DNS_request[i] & 0xC000) /* pointer */
				break;
			else
				i += DNS_request[i] + 1; /* got to next lenght specifier*/
		}

		i += 5; /* go to the next section*/
	}



	/* handle DNS responses */
	for (dns_answers_index = 0; dns_answers_index < dns_answers && dns_answers_index < recv_len; dns_answers_index++){

		/* skip though the name */
		while(DNS_request[i] != 0){
			
			if (DNS_request[i] & 0xC000){ /* pointer */
				i++;
				break;
			}else
				i += DNS_request[i] + 1; /* got to next lenght specifier*/
			
		}
		i++; /* go past the name */

		/* check TYPE */
		if (DNS_request[i] == 0 && DNS_request[i+1] == 1){ /* if we found an A record */
			state = DLS_found;
		}else{
			state = DLS_skipping;
		}
		i += 8; /* skip Type and TTL */
		
		rdlength = DNS_request[i+1] | (DNS_request[i] << 8); i += 2;

		if (state == DLS_skipping || rdlength != 4){ /* we are either not interested in the data, or the data is NOT a ipv4*/
			i += rdlength;
		}else{
			/*This is the IPV4 we want!*/

			ip_address = (DNS_request[i+3] <<  0);
			ip_address |= (DNS_request[i+2] <<  8);
			ip_address |= (DNS_request[i+1] << 16);
			ip_address |= (DNS_request[i+0] << 24);

			free(DNS_request);
			close(sock);
			return ip_address;
		}
	}

	/* we dit not find any results */
	goto retry_with_new_DNS;

DNS_lookup_cleanup:
	free(DNS_request);
	close(sock);
	return 0;
}

char* download_file(char* url, int32* DNS_LIST, FILE* log){
	puts("Not implemented");
	return NULL;
}

/* POSIX */
#endif

