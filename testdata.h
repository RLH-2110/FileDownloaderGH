#ifndef included_testdata_h
#define included_testdata_h

#include "int.h"

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef true
typedef int bool;
#define true 0x01
#define false 0x0
#endif

#define num_parse_URL_Tests 20
 char* testUrls[num_parse_URL_Tests] = {
	"https://www.github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt", /* 01 */
	"https://github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
	"www.github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
	"github.com/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
	"https://www.github.com",  /* 05 */
	"https://github.com",
	"www.github.com",
	"github.com",
	NULL,
	"()()", /* 10 */
	"..",
	".",
	"abn.. / ",
	"https://abc.deg.hij..lhagas.asgdsag/test",
	"abc.deg.hij.lhagas.asgdsag", /* 15 */
	"https://abc.deg.hij.lhagas.asgdsag/test",
	"abc.deg.hij.lhagas.asgdsag/test",
	"https://abc.deg.hij.lhagas.asgdsag",
	"http://www.github.com",
	"ftp://www.github.com" /* 20 */
};

 char* expectedHostname[num_parse_URL_Tests] = {
	"www.github.com", /* 01 */
	"www.github.com",
	"www.github.com",
	"www.github.com",
	"www.github.com",/* 05 */
	"www.github.com",
	"www.github.com",
	"www.github.com",
	NULL,
	NULL,/* 10 */
	NULL,
	NULL,
	NULL,
	NULL,
	"abc.deg.hij.lhagas.asgdsag",/* 15 */
	"abc.deg.hij.lhagas.asgdsag",
	"abc.deg.hij.lhagas.asgdsag",
	"abc.deg.hij.lhagas.asgdsag",
	"www.github.com",
	"www.github.com" /* 20 */
};

 char* expectedProtocol[num_parse_URL_Tests] = {
	"https", /* 01 */
	"https",
	"https",
	"https",
	"https",  /* 05 */
	"https",
	"https",
	"https",
	NULL,
	NULL,/* 10 */
	NULL,
	NULL,
	NULL,
	NULL,
	"https", /* 15 */
	"https",
	"https",
	"https",
	"http",
	"ftp" /* 20 */
};

 char* expectedRest[num_parse_URL_Tests] = {
	"/RLH-2110/FileDownloaderGH/blob/master/sample.txt", /* 01 */
	"/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
	"/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
	"/RLH-2110/FileDownloaderGH/blob/master/sample.txt",
	"", /* 05 */
	"",
	"",
	"",
	NULL,
	NULL,  /* 10 */
	NULL,
	NULL,
	NULL,
	NULL,
	"", /* 15 */
	"/test",
	"/test",
	"",
	"",
	"" /* 20 */
};




#define num_QNAME_Tests 5

 char* testHostnames[num_QNAME_Tests] = {
	"www.github.com",
	"github",
	"",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"ABC.DEF.GHI.JKL.asg-gasdg.sd.d.hsdhsdh.g"
};

  unsigned char expectedQname0[] = { 3, 'w', 'w', 'w', 6, 'g', 'i', 't', 'h', 'u', 'b', 3, 'c', 'o', 'm', 0 };
  unsigned char expectedQname1[] = { 6, 'g', 'i', 't', 'h', 'u', 'b', 0 };
  unsigned char expectedQname2[] = { 0 };
/* NULL */
  unsigned char expectedQname4[] = { 3, 'A', 'B', 'C', 3, 'D', 'E', 'F', 3, 'G', 'H', 'I', 3, 'J', 'K', 'L', 9, 'a', 's', 'g', '-', 'g', 'a', 's', 'd', 'g', 2, 's', 'd', 1, 'd', 7, 'h', 's', 'd', 'h', 's', 'd', 'h', 1, 'g', 0 };

 unsigned char* expectedQNAME[num_QNAME_Tests] = {
	expectedQname0,
	expectedQname1,
	expectedQname2,
	NULL,
	expectedQname4
};



#define num_DNSCMP_Tests 12
 /*
  char DNScmp1[] = { 0xDB,0x42,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x03,0x65,0x64,0x75,0x00,0x00,0x01,0x00,0x01 };
  char DNScmp2[] = { 0xDB,0x42,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x04,0x65,0x64,0x75,0x75,0x00,0x00,0x01,0x00,0x01 };
  char DNScmp3[] = { 0xDB,0x42,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x03,0x65,0x64,0x75,0x00,0x00,0x01,0x21,0x01 };
  char DNScmp4[] = { 0xDB,0x42,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x03,0x66,0x64,0x75,0x00,0x00,0x01,0x21,0x01 };
  char DNScmp5[] = { 0xDB,0x42,0x21,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x03,0x65,0x64,0x75,0x00,0x00,0x01,0x21,0x01 };
 
	I got -pedantic -Wall on, and immidiate values are ints. compiler warns me. so here is a ""Correct"" version created via (0x[A-Z0-9]{2}) replaced as (char)$1 with regex
 */

 unsigned char DNScmp1[] = { (char)0xDB,(char)0x42,(char)0x01,(char)0x00,(char)0x00,(char)0x01,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x03,(char)0x77,(char)0x77,(char)0x77,(char)0x0C,(char)0x6E,(char)0x6F,(char)0x72,(char)0x74,(char)0x68,(char)0x65,(char)0x61,(char)0x73,(char)0x74,(char)0x65,(char)0x72,(char)0x6E,(char)0x03,(char)0x65,(char)0x64,(char)0x75,(char)0x00,(char)0x00,(char)0x01,(char)0x00,(char)0x01 };
 unsigned char DNScmp2[] = { (char)0xDB,(char)0x42,(char)0x01,(char)0x00,(char)0x00,(char)0x01,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x03,(char)0x77,(char)0x77,(char)0x77,(char)0x0C,(char)0x6E,(char)0x6F,(char)0x72,(char)0x74,(char)0x68,(char)0x65,(char)0x61,(char)0x73,(char)0x74,(char)0x65,(char)0x72,(char)0x6E,(char)0x04,(char)0x65,(char)0x64,(char)0x75,(char)0x75,(char)0x00,(char)0x00,(char)0x01,(char)0x00,(char)0x01 };
 unsigned char DNScmp3[] = { (char)0xDB,(char)0x42,(char)0x01,(char)0x00,(char)0x00,(char)0x01,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x03,(char)0x77,(char)0x77,(char)0x77,(char)0x0C,(char)0x6E,(char)0x6F,(char)0x72,(char)0x74,(char)0x68,(char)0x65,(char)0x61,(char)0x73,(char)0x74,(char)0x65,(char)0x72,(char)0x6E,(char)0x03,(char)0x65,(char)0x64,(char)0x75,(char)0x00,(char)0x00,(char)0x01,(char)0x21,(char)0x01 };
 unsigned char DNScmp4[] = { (char)0xDB,(char)0x42,(char)0x01,(char)0x00,(char)0x00,(char)0x01,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x03,(char)0x77,(char)0x77,(char)0x77,(char)0x0C,(char)0x6E,(char)0x6F,(char)0x72,(char)0x74,(char)0x68,(char)0x65,(char)0x61,(char)0x73,(char)0x74,(char)0x65,(char)0x72,(char)0x6E,(char)0x03,(char)0x66,(char)0x64,(char)0x75,(char)0x00,(char)0x00,(char)0x01,(char)0x21,(char)0x01 };
 unsigned char DNScmp5[] = { (char)0xDB,(char)0x42,(char)0x21,(char)0x00,(char)0x00,(char)0x01,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x03,(char)0x77,(char)0x77,(char)0x77,(char)0x0C,(char)0x6E,(char)0x6F,(char)0x72,(char)0x74,(char)0x68,(char)0x65,(char)0x61,(char)0x73,(char)0x74,(char)0x65,(char)0x72,(char)0x6E,(char)0x03,(char)0x65,(char)0x64,(char)0x75,(char)0x00,(char)0x00,(char)0x01,(char)0x21,(char)0x01 };


 unsigned char* test_DNScmpA[num_DNSCMP_Tests] = {
	DNScmp1,
	NULL,
	NULL,
	DNScmp1,
	DNScmp1,
	DNScmp1,
	DNScmp1,
	DNScmp1,
	DNScmp2,
	DNScmp3,
	DNScmp4,
	DNScmp5,
};
 unsigned char* test_DNScmpB[num_DNSCMP_Tests] = {
	DNScmp1,
	NULL,
	DNScmp1,
	NULL,
	DNScmp2,
	DNScmp3,
	DNScmp4,
	DNScmp5,
	DNScmp2,
	DNScmp3,
	DNScmp4,
	DNScmp5,
};

 int excpetedDNScmp[num_DNSCMP_Tests] = {
	true,
	true,
	false,
	false,
	false,
	false,
	false,
	false,
	true,
	true,
	true,
	true,
};





#define num_DNSREQPRINT_Tests 1
 /*char test_DNSreq1[] = {0xDB,0x42,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x03,0x65,0x64,0x75,0x00,0x00,0x01,0x00,0x01};*/
 unsigned char test_DNSreq1[] = { (char)0xDB,(char)0x42,(char)0x01,(char)0x00,(char)0x00,(char)0x01,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x03,(char)0x77,(char)0x77,(char)0x77,(char)0x0C,(char)0x6E,(char)0x6F,(char)0x72,(char)0x74,(char)0x68,(char)0x65,(char)0x61,(char)0x73,(char)0x74,(char)0x65,(char)0x72,(char)0x6E,(char)0x03,(char)0x65,(char)0x64,(char)0x75,(char)0x00,(char)0x00,(char)0x01,(char)0x00,(char)0x01 };

 unsigned char* test_DNSRequestPrints[num_DNSREQPRINT_Tests] = {
	test_DNSreq1
};

 unsigned char expectedDNSreqprint[] = "ID: 56130\nQDCOUNT: 1\nANCOUNT: 0\nNSCOUNT: 0\nARCOUNT: 0\nFLAGS:\n\tQR: 0\n\tOpcode: 0\n\tAA: 0\n\tTC: 0\n\tRD: 1\n\tRA: 0\n\tZ: 0\n\tRCODE: 0\nQNAME: 3www12northeastern3edu\nQTYPE: 1\nQCLASS: 1\n";
 unsigned char* expectedDNSRequestPrints[num_DNSREQPRINT_Tests] = {
	expectedDNSreqprint
};





#define num_DNSREQ_Tests 1
#define DNSREQ_test_ID ((0xdb << 8)+0x42)

 char* test_DNSreq_hostnames[] = { "www.northeastern.edu" };

 unsigned char expectedDNSreq1[] = { (char)0xDB,(char)0x42,(char)0x01,(char)0x00,(char)0x00,(char)0x01,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x00,(char)0x03,(char)0x77,(char)0x77,(char)0x77,(char)0x0C,(char)0x6E,(char)0x6F,(char)0x72,(char)0x74,(char)0x68,(char)0x65,(char)0x61,(char)0x73,(char)0x74,(char)0x65,(char)0x72,(char)0x6E,(char)0x03,(char)0x65,(char)0x64,(char)0x75,(char)0x00,(char)0x00,(char)0x01,(char)0x00,(char)0x01 };

 unsigned char* expectedDNSRequest[num_DNSREQ_Tests] = {
	expectedDNSreq1
};

 int expectedDNSRequestSize[num_DNSREQ_Tests] = {
   38
 };


#define num_IPv4ToString_Tests 2
int32 test_str_ips[num_IPv4ToString_Tests] = { 0x000203FF, 0xFFFFFFFF };
char* expected_strIPs[num_IPv4ToString_Tests] = { "0.2.3.255", "255.255.255.255" };


#define num_DNS_lookup_Tests 5
char* test_DNS_lookups[num_DNS_lookup_Tests] = { "www.google.com", "github.com", "raw.githubusercontent.com" , "", NULL}; /* we do not test more cases, since they are undefined behavior and not my problem*/
bool find_expected_DNS_lookups[num_DNS_lookup_Tests] = { true, true, true, false, false};


#define num_donwloader_Tests 3
char downloader_test_url1[] = "https://raw.githubusercontent.com/RLH-2110/FileDownloaderGH/refs/heads/master/sample.txt";
char downloader_test_url2[] = "https://raw.githubusercontent.com/RLH-2110/FileDownloaderGH/refs/heads/master/img_sample.png";


char* downloader_test_urls[num_donwloader_Tests] = { 
		NULL, 
		downloader_test_url1,
	 	downloader_test_url2
};

uint32 expected_filesize[num_donwloader_Tests] = {0,78,1289240};
char* compareFileAgainst[num_donwloader_Tests] = {"","sample.txt","img_sample.png"};


/*included_downloader_h*/
#endif