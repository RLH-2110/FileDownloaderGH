#ifndef included_defines_h
#define included_defines_h


#ifndef NULL
#define NULL (void*)0
#endif

#ifndef true
typedef int bool;
#define true 0xFF
#define false 0x0
#endif


#define putslog(str)\
if(log != NULL){\
	fputs(str,log);\
	fputs("\n",log);\
}
#define perrorlog(str)\
if(log != NULL){\
	fputs(str,log);\
	fputs(" ",log);\
	fputs(strerror(errno),log);\
	fputs("\n",log); \
}
#define printflog(str,var)\
if(log != NULL){\
	fprintf(log,str,var);\
}

/*included_defines_h*/
#endif