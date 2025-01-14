#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <errno.h>

#include "int.h"

#include "downloader.h"
#include "DNS_offsets.h"
#include "defines.h"



#ifdef WINDOWS

	#include <winsock2.h>

	int32 DNS_lookup(char* url, int32* DNS_LIST, FILE* log){
		puts("Not implemented");
		return 0;
	}

	char* download_file(char* url, int32* DNS_LIST, FILE* log){
		puts("Not implemented");
		return NULL;
	}


#endif