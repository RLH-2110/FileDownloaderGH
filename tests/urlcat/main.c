#include "../../downloader.h"
#include "../../int.h"
#include <stdio.h>
#include <errno.h>

#ifndef NULL
#define NULL (void*)0
#endif

/* gets a sting, if its NULl it retuns "NULL", if its empty it returns "(EMPTY)" if its neither, it returns the given string*/
unsigned char* got(unsigned char* str) {
	if (str == NULL)
		return (unsigned char*)"NULL";
	if (str[0] == 0)
		return (unsigned char*)"(EMPTY)";
	return str;
}

int main(int argc, char* argv[]){
	unsigned char* content = NULL;
	uint32 filesize = 0;
	int32 DNS_LIST[] = {
	0x08080808, /* 8.8.8.8 google dns*/
	0x08080404, /* 8.8.4.4 google dns*/
	0x7F000035, /* 172.0.0.53 some private DNS in a local network where google DNS is blocked*/
	0xAC10020C, /* 127.16.2.12 same as above*/
	0 /* null termination */
	};
		
	if (argc != 2){
		printf("useage: %s [URL]",argv[0]);
	}
	
	downloader_init();
	
	errno = 0;
	content = download_file(argv[1], DNS_LIST, &filesize, NULL);
	if (errno != 0){
		if (errno == EINVAL){
			puts("the url was considered invalid! (reminder: https only)");
		}else{
			perror("error occured: ");
		}
		
		donwloader_cleanup();
		return 0;
	}
		
	printf("file size: %d\n\nFile:\n%s\n",filesize,got(content));
	
	donwloader_cleanup();
	
	return 0;
}