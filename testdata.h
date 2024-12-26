#ifndef included_testdata_h
#define included_testdata_h

#ifndef NULL
#define NULL (void*)0
#endif

#ifndef true
typedef int bool;
#define true 0xFF
#define false 0x0
#endif

#define num_parse_URL_Tests 20
extern char* testUrls[num_parse_URL_Tests] = {
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

extern char* expectedHostname[num_parse_URL_Tests] = {
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

extern char* expectedProtocol[num_parse_URL_Tests] = {
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

extern char* expectedRest[num_parse_URL_Tests] = {
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

extern char* testHostnames[num_QNAME_Tests] = {
	"www.github.com",
	"github",
	"",
	"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
	"ABC.DEF.GHI.JKL.asg-gasdg.sd.d.hsdhsdh.g"
};

extern const char expectedQname0[] = { 3, 'w', 'w', 'w', 6, 'g', 'i', 't', 'h', 'u', 'b', 3, 'c', 'o', 'm', 0 };
extern const char expectedQname1[] = { 6, 'g', 'i', 't', 'h', 'u', 'b', 0 };
extern const char expectedQname2[] = { 0 };
/* NULL */
extern const char expectedQname4[] = { 3, 'A', 'B', 'C', 3, 'D', 'E', 'F', 3, 'G', 'H', 'I', 3, 'J', 'K', 'L', 9, 'a', 's', 'g', '-', 'g', 'a', 's', 'd', 'g', 2, 's', 'd', 1, 'd', 7, 'h', 's', 'd', 'h', 's', 'd', 'h', 1, 'g', 0 };

extern char* expectedQNAME[num_QNAME_Tests] = {
	expectedQname0,
	expectedQname1,
	expectedQname2,
	NULL,
	expectedQname4
};



#define num_DNSCMP_Tests 12

extern const char DNScmp1[] = { 0xDB,0x42,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x03,0x65,0x64,0x75,0x00,0x00,0x01,0x00,0x01 };
extern const char DNScmp2[] = { 0xDB,0x42,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x04,0x65,0x64,0x75,0x75,0x00,0x00,0x01,0x00,0x01 };
extern const char DNScmp3[] = { 0xDB,0x42,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x03,0x65,0x64,0x75,0x00,0x00,0x01,0x21,0x01 };
extern const char DNScmp4[] = { 0xDB,0x42,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x03,0x66,0x64,0x75,0x00,0x00,0x01,0x21,0x01 };
extern const char DNScmp5[] = { 0xDB,0x42,0x21,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x03,0x65,0x64,0x75,0x00,0x00,0x01,0x21,0x01 };

extern char* test_DNScmpA[num_DNSCMP_Tests] = {
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
extern char* test_DNScmpB[num_DNSCMP_Tests] = {
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

extern int excpetedDNScmp[num_DNSCMP_Tests] = {
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
extern const char test_DNSreq1[] = { 0xDB,0x42,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x03,0x65,0x64,0x75,0x00,0x00,0x01,0x00,0x01 };
extern char* test_DNSRequestPrints[num_DNSREQPRINT_Tests] = {
	test_DNSreq1
};

extern const char expectedDNSreqprint[] = "ID: 27648\nQDCOUNT: 0\nANCOUNT: 0\nNSCOUNT: 0\nARCOUNT: 0\nFLAGS:\n\tQR: 0\n\tOpcode: 0\n\tAA: 0\n\tTC: 0\n\tRD: 1\n\tRA: 0\n\tZ: 0\n\tRCODE: 0\nQNAME: 3www12northeastern3edu\nQTYPE: 0\nQCLASS: 0\n";
extern char* expectedDNSRequestPrints[num_DNSREQPRINT_Tests] = {
	expectedDNSreqprint
};





#define num_DNSREQ_Tests 1
#define DNSREQ_test_ID ((0xdb << 8)+0x42)

extern char* test_DNSreq_hostnames[] = { "www.northeastern.edu" };

extern const char expectedDNSreq1[] = { 0xDB,0x42,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x77,0x77,0x77,0x0C,0x6E,0x6F,0x72,0x74,0x68,0x65,0x61,0x73,0x74,0x65,0x72,0x6E,0x03,0x65,0x64,0x75,0x00,0x00,0x01,0x00,0x01 };
extern char* expectedDNSRequest[num_DNSREQ_Tests] = {
	expectedDNSreq1
};

/*included_downloader_h*/
#endif