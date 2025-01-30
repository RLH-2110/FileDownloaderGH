#include <stdlib.h>
#include <stdio.h>

#include "../defines.h"
#include "../int.h"
#include "urlParse.h"
#include "urlParse_testData.h"


#include "../tests/testing_funcs.h"

bool test_urlParse(uint32* out_tests_passed, uint32* out_tests_amount, bool print) {
	uint32 i;
	uint32 passed = 0;
	parsedUrl* result;
	
	if (out_tests_amount != NULL)
		*out_tests_amount = num_parse_URL_Tests;


	for (i = 0; i < num_parse_URL_Tests;i++) {
		result = parse_URL(testUrls[i]);



		/* check if NULL,*/

		if (result == NULL) {
			if (expectedHostname[i] == NULL) {
				passed++;
				continue;
			}
			else {
				if (print)
					printf("parse_URL test %u failed\nexpected data\nbut got NULL\n", i + 1);

				goto test_urlParse_end;
			}
		}
		else {
			if (expectedHostname[i] == NULL) {
				if (print) 
					printf("parse_URL test %u failed\nexpected NULL\nbut got data\n", i + 1);

				goto test_urlParse_end;
			}
		}



		/* check hostname (TEST A)*/
		if (compare(result->hostname, expectedHostname[i]) != 0) {
			if (print) 
				printf("parse_URL test %u A (hostname) failed\nexpected: %s\ngot: %s\n", i + 1, got(expectedHostname[i]), got(result->hostname));
			
			goto test_urlParse_end;
		}


		/* check protocol (TEST B)*/
		if (compare(result->protocol, expectedProtocol[i]) != 0) {
			if (print)
				printf("parse_URL test %u B (protocol) failed\nexpected: %s\ngot: %s\n", i + 1, got(expectedProtocol[i]), got(result->protocol));
			
			goto test_urlParse_end;
		}


		/* check path (TEST C)*/
		if (compare(result->path, expectedPath[i]) != 0) {
			if (print)
				printf("parse_URL test %u C (path) failed\nexpected: %s\ngot: %s\n", i + 1, got(expectedPath[i]), got(result->path));
			
			goto test_urlParse_end;
		}

		/* check port (TEST D)*/
		if (result->port != expectedPort[i]) {
			if (print)
				printf("parse_URL test %u D (port) failed\nexpected: %u\ngot: %u\n", i + 1, expectedPort[i], result->port);

			goto test_urlParse_end;
		}

		free(result); result = NULL;
		passed++;
	}

	test_urlParse_end:

	if (out_tests_passed != NULL)
		*out_tests_passed = passed;

	if (passed == num_parse_URL_Tests)
		return true;
	else
		return false;

}

/* we dont need the define anymore, after we used it in this function*/
#undef num_parse_URL_Tests