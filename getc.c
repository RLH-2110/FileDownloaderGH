#include <stdio.h>

#ifdef WINDOWS
#include <conio.h>
char pgetc() {
	return getch();
}
#endif /* WINDOWS */

#ifdef POSIX

#include <termios.h>
#include <unistd.h>

char pgetc() {
	/*puts("pgetc not Implememted");
	return 0;*/


    struct termios oldattr, newattr;
    int ch;

    /* Get the current terminal attributes */
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;

    /* Set the terminal to non-canonical mode without echo */
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

    /* Read a single character */
    ch = getchar();

    /* Restore the original terminal attributes */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

    #ifndef NO_PGETC_BACKSPACE_FIX
        if (ch == 0x7F) /* posix might send 0x7F when pressing backspace instead of 0x08 */
            ch = 0x08;
    #endif


    return ch;

}
#endif /* POSIX */

