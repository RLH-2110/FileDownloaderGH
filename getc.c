#include <stdio.h>

#ifdef WINDOWS
#include <conio.h>
char pgetc() {
	return getch();
}
#endif // WINDOWS

#ifdef POSIX
char pgetc() {
	puts("pgetc not Implememted");
	return 0;
}
#endif // POSIX

