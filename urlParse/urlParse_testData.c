/* testdata: */

#include "../defines.h"
#include "../int.h"

#define num_parse_URL_Tests 51
char* testUrls[num_parse_URL_Tests] = {
   "https://www.github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt", /* 01 */
   "https://github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
   "www.github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
   "github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
   "https://www.github.com",					/* 05 */
   "https://github.com",
   "www.github.com",
   "github.com",
   NULL,
   "()()",										/* 10 */
   "..",
   ".",
   "abn.. / ",
   "https://abc.deg.hij..lhagas.asgdsag/test",
   "abc.deg.hij.lhagas.asgdsag",				/* 15 */
   "https://abc.deg.hij.lhagas.asgdsag/test",
   "abc.deg.hij.lhagas.asgdsag/test",
   "https://abc.deg.hij.lhagas.asgdsag",
   "https://www.github.com:8080",
   NULL,										/* 20 */
   "https://example.com:8080/path",
   "https://example.com:65535",
   "http://example.com",
   "ftp://example.com",
   "https://192.168.1.1",                       /* 25 */
   "https://[2001:db8::1]",
   "https://example.com:abc",
   "https://example.com:65536",
   "https://example.com:0",
   "https://user@example.com",                  /* 30 */
   "https://invalid_host.com",
   "https:///path",
   "https://example.com/",
   "https://example.com?query=1",
   "https://example.com#fragment",              /* 35 */
   "https://example.com/path%20with%20space",
   "https://example.com/path with space",
   "https://exa:mple.com",
   "https://example.com./path",
   "https://.example.com",                      /* 40 */
   "https://ex..ample.com",
   "https://a",
   "https://verylonghostname.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.abcdefghijklmnopqrstuvwxyz.com",
   "https://example.com//path//to//file",
   "https://example.com:8080",                  /* 45 */
   "https:example.com",
   "https://ex,ample.com",
   "https://example.com/path/../test",
   "https://localhost:8080",
   "https://example.com:0/path",                /* 50 */
   "https://example.com:-80",
};


/* we check against NULL pointers by seeing if any atribute is NULL, becuse no attribute can ever natrually be NULL.*/
 /* the numbers next to the data is the index of the element + 1. this is so when we see that test x has failed, we can quickly find it here*/

char* expectedHostname[num_parse_URL_Tests] = {
	"www.github.com",/* 01 */
	"www.github.com",
	"www.github.com",
	"www.github.com",
	"www.github.com",/* 05 */
	"www.github.com",
	"www.github.com",
	"www.github.com",
	NULL,
	NULL,			 /* 10 */
	NULL,
	NULL,
	NULL,
	NULL,
	"abc.deg.hij.lhagas.asgdsag",	/* 15 */
	"abc.deg.hij.lhagas.asgdsag",
	"abc.deg.hij.lhagas.asgdsag",
	"abc.deg.hij.lhagas.asgdsag",
	"www.github.com",
	NULL,			/* 20 */
	"example.com",
	"example.com",
	NULL,
	NULL,
	NULL,			/* 25 */
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,           /* 30 */
	NULL,
	NULL,
	"example.com",
	"example.com",
	"example.com",  /* 35 */
	"example.com",
	NULL,
	NULL,
	"example.com.",
	NULL,           /* 40 */
	NULL,
	NULL,
	NULL,
	"example.com",
	"example.com",  /* 45 */
	NULL,
	NULL,
	"example.com",
	"localhost",
	NULL,			/* 50 */
	NULL
};

char* expectedProtocol[num_parse_URL_Tests] = {
	"https",		/* 01 */
	"https",
	"https",
	"https",
	"https",		/* 05 */
	"https",
	"https",
	"https",
	NULL,
	NULL,			/* 10 */
	NULL,
	NULL,
	NULL,
	NULL,
	"https",		/* 15 */
	"https",
	"https",
	"https",
	"https",
	NULL,			/* 20 */
	"https",
	"https",
	NULL,
	NULL,
	NULL,			/* 25 */
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,           /* 30 */
	NULL,
	NULL,
	"https",
	"https",
	"https",		/* 35 */
	"https",
	NULL,
	NULL,
	"https",
	NULL,           /* 40 */
	NULL,
	NULL,
	NULL,
	"https",
	"https",		/* 45 */
	NULL,
	NULL,
	"https",
	"https",
	NULL,			/* 50 */
	NULL

};

char* expectedRest[num_parse_URL_Tests] = {
   "/RLH-2110/FileDownloaderGH/blob/master/sample.txt", /* 01 */
   "/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
   "/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
   "/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
   "",				/* 05 */
   "",
   "",
   "",
   NULL,
   NULL,			/* 10 */
   NULL,
   NULL,
   NULL,
   NULL,
   "",				/* 15 */
   "/test",
   "/test",
   "",
   "",
   NULL,			/* 20 */
   "/path",
   "/",
   NULL,
   NULL,
   NULL,			/* 25 */
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,			/* 30 */
   NULL,
   NULL,
   "/",
   "/",
   "/",				/* 35 */
   "/path%20with%20space",
   NULL,
   NULL,
   "/",
   NULL,           /* 40 */
   NULL,
   NULL,
   NULL,
   "//path//to//file",
   "/",				/* 45 */
   NULL,
   NULL,
   "/path/../test",
   "/",
   NULL,			/* 50 */
   NULL

};

uint32 expectedPort[num_parse_URL_Tests] = {
	 443, /* 01 */
	 443,
	 443,
	 443,
	 443, /* 05 */
	 443,
	 443,
	 443,
	   0,
	   0, /* 10 */
	   0,
	   0,
	   0,
	   0,
	 443, /* 15 */
	 443,
	 443,
	 443,
	8080,
	   0, /* 20 */
	8080,
   65535,
	   0,
	   0,
	   0, /* 25 */
	   0,
	   0,
	   0,
	   0,
	   0, /* 30 */
	   0,
	   0,
	 443,
	 443,
	 443, /* 35 */
	 443,
	   0,
	   0,
	 443,
	   0, /* 40 */
	   0,
	   0,
	   0,
	 443,
	8080, /* 45 */
	   0,
	   0,
	 443,
	8080,
	   0, /* 50 */
	   0
};


