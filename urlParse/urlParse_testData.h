#ifndef included_urlParse_testData_h
#define included_urlParse_testData_h

#include "../int.h"


/* tests if urlParse works

	out_tests_passed: If not NULL, will be set to the amount of passed tests
		
	out_tests_amount: If not NULL, it will be set to the total amount of tests
		
	print: if set to true, messages will be printed if tests fail.

	retuns:		true if all tests passed
				false if any test failed

*/
bool test_urlParse(uint32* out_tests_passed, uint32* out_tests_amount, bool print);




/* testdata: */


#define num_parse_URL_Tests 51
extern char* testUrls[num_parse_URL_Tests];
extern expectedHostname[num_parse_URL_Tests];
extern char* expectedProtocol[num_parse_URL_Tests];
extern char* expectedRest[num_parse_URL_Tests];
extern uint32 expectedPort[num_parse_URL_Tests];


/*included_urlParse_testData_h*/
#endif