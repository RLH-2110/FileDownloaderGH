
#include <stdlib.h>
#include "defines.h"
#include <stdio.h>
#include "int.h";

#define oI_buffsize 4

bool isNum(char chr) {
	if (chr < '0' || chr > '9')
		return false;
	return true;
}

void shift_input_buff(char* buff, int super_i) {
	int shifts = oI_buffsize - super_i - 1;
	int i;

	for (i = oI_buffsize - 2; i >= 0; i--) {

		if (i - shifts >= 0) {
			buff[i] = buff[i - shifts];
		}
		else {
			buff[i] = '0';
		}
	}
}

/* fgets does weird shit in bash, so I make my own input thing
this also has the advantage of me being able to block stuff that is not numbers*/
void get_input(char* buff, size_t lengh, FILE* steam) {
	char c;
	size_t i;
	for (i = 0;i < lengh - 1;i++) {

		c = pgetc();
		if (c == '\n' || c == EOF || c == '.' | c == '\r') {
			buff[i] = '\0';
			break;
		}

		if (c == 0x8) { /* backspace */
			i -= 2;

			if (i > lengh && i != -1) { /* underflow */
				i = -1; /* i will be incremented afterwards, so it will reset to 0*/
				continue;
			}

			/* delete old input*/
			fputc(c, stdout);
			fputc(' ', stdout);
			fputc(c, stdout);

			continue;
		}
		if (isNum(c) == false) {
			/* redo getting char*/
			--i;
			continue;
		}

		buff[i] = c;
		fputc(c, stdout);
	}

	buff[lengh - 1] = '\0';
}

int8 octetInput(void) {


	char buff[oI_buffsize];
	uint8 val;
	int i;
	int c;

octetInput_restart:

	/* loop till all the parts are numbers*/

	fputs(": ", stdout);
	get_input(buff, oI_buffsize, stdin);

	// count the number of inputs
	for (i = 0; i < oI_buffsize - 1; i++) {
		if (buff[i] == '\0')
			break;
	}

	if (i != oI_buffsize - 1) { /* if the user pressed return*/
		shift_input_buff(buff, i);
	}

	/* now we have exacty 3 characters that are numbers! */

	/* check that we have not exceted the limit! */
	if (buff[0] > '2')
		goto octetInput_restart;
	else if (buff[0] == '2') {

		if (buff[1] > '5')
			goto octetInput_restart;
		else if (buff[1] == '5')
			if (buff[2] > '5')
				goto octetInput_restart;
	}

	val =
		(buff[2] - '0')
		+ (buff[1] - '0') * 10
		+ (buff[0] - '0') * 100;

	return val;
}

int32 ipv4Input(void) {

	int32 ip;
	char* debugprint;

	fputs("fist (msb) octet", stdout);
	ip = (octetInput() << 24);
	fputs("\nsecond octet", stdout);
	ip |= (octetInput() << 16);
	fputs("\nthird octet", stdout);
	ip |= (octetInput() << 8);
	fputs("\nlast octet", stdout);
	ip |= (octetInput() << 0);

	debugprint = IPv4ToString(ip);
	printf("\nip: %s\n", debugprint);
	free(debugprint); debugprint = NULL;

	return ip;
}
