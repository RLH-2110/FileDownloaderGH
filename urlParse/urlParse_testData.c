/* testdata: */

#include "../defines.h"
#include "../int.h"

/* "header:" */

#define num_parse_URL_Tests 63
extern char* testUrls[num_parse_URL_Tests];
extern char* expectedHostname[num_parse_URL_Tests];
extern char* expectedProtocol[num_parse_URL_Tests];
extern char* expectedPath[num_parse_URL_Tests];
extern uint16 expectedPort[num_parse_URL_Tests];


/* code : */

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
	"https://www.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.com",
	"https://www.AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA.com",
	"https://example-.com/",					 /* 55 */
	"https://-example.com/",
	"https://exam-ple.com/",
	"HTTPs://www.google.com",
	"https://www.youtube.com/results?search_query=testy+test+test+test",
	"https://example.com/page?var1=value1&var2=value2&var3=value3", /* 60 */
	"https://example.com?var1=value1&var2=value2&var3=value3",
	"google",
	"https://example.com/wiki#comments",
	"https://www.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.com",
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
	"www.example.com",
	"www.example.com",
	NULL,
	NULL,
	"192.168.1.1",	/* 25 */
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,           /* 30 */
	NULL,
	NULL,
	"www.example.com",
	"www.example.com",
	"www.example.com",  /* 35 */
	"www.example.com",
	NULL,
	NULL,
	"www.example.com",
	NULL,           /* 40 */
	NULL,
	"www.a",
	NULL,
	"www.example.com",
	"www.example.com",  /* 45 */
	NULL,
	NULL,
	"www.example.com",
	"www.localhost",
	NULL,			/* 50 */
	NULL,
	"www.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.com",
	NULL,
	NULL,			/* 55 */
	NULL,
	"www.exam-ple.com",
	"www.google.com",
	"www.youtube.com",
	"www.example.com", /* 60 */
	"www.example.com",
	"www.google",
	"www.example.com",
	NULL,
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
	"https",		/* 25 */
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
	"https",
	NULL,
	"https",
	"https",		/* 45 */
	NULL,
	NULL,
	"https",
	"https",
	NULL,			/* 50 */
	NULL,
	"https",
	NULL,
	NULL,			/* 55 */
	NULL,
	"https",
	"HTTPs",
	"https",
	"https",		/* 60 */
	"https",
	"https",
	"https",
	NULL,

};

char* expectedPath[num_parse_URL_Tests] = {
	"/RLH-2110/FileDownloaderGH/blob/master/sample.txt", /* 01 */
	"/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
	"/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
	"/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
	"/",			/* 05 */
	"/",
	"/",
	"/",
	NULL,
	NULL,			/* 10 */
	NULL,
	NULL,
	NULL,
	NULL,
	"/",			/* 15 */
	"/test",
	"/test",
	"/",
	"/",
	NULL,			/* 20 */
	"/path",
	"/",
	NULL,
	NULL,
	"/",			/* 25 */
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,			/* 30 */
	NULL,
	NULL,
	"/",
	"/",
	"/",			/* 35 */
	"/path%20with%20space",
	NULL,
	NULL,
	"/path",
	NULL,           /* 40 */
	NULL,
	"/",
	NULL,
	"//path//to//file",
	"/",			/* 45 */
	NULL,
	NULL,
	"/path/../test",
	"/",
	NULL,			/* 50 */
	NULL,
	"/",
	NULL,
	NULL,			/* 55 */
	NULL,
	"/",
	"/",
	"/results",
	"/page",		/* 60 */
	"/",
	"/",
	"/wiki",
	NULL,
};

uint16 expectedPort[num_parse_URL_Tests] = {
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
	 443, /* 25 */
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
	 443,
	   0,
	 443,
	8080, /* 45 */
	   0,
	   0,
	 443,
	8080,
	   0, /* 50 */
	   0,
	 443,
	   0,
	   0, /* 55 */
	   0,
	 443,
	 443,
	 443,
	 443, /* 60 */
	 443,
	 443,
	 443,
	   0,
};


