#include "urlParse/urlParse.c"

typedef char uchar;

#define mallocOrExit(ptr,size) \
	ptr = malloc(size); \
	if (ptr == NULL){ \
		puts("Out of mem!");\
		exit(EXIT_FAILURE);\
	}

#include "helpers.c"
#include "main_common.c"
#include "main_posix.c"
#include "main_win.c"