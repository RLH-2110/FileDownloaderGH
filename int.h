#ifndef intH
#define intH

#include <limits.h>


	/* (u)int8 */ 
	#if(UCHAR_MAX != 0xff)
		#error UNSIGNED CHAR IS NOT A BYTE!
	#else
		typedef unsigned char uint8;
	#endif

	#if(SCHAR_MAX != 0x7f)
		#error CHAR IS NOT A BYTE!
	#else
		typedef char int8;
	#endif



	/* (u)int16 */ 
	#if(USHRT_MAX != 0xffff)
		#error UNSIGNED SHORT IS NOT A WORD!
	#else
		typedef unsigned short uint16;
	#endif

	#if(SHRT_MAX != 0x7fff)
		#error SHORT IS NOT A WORD!
	#else
		typedef short int16;
	#endif


	/* (u)int32 */ 
	#if(UINT_MAX != 0xffffffff)
		#if (ULONG_MAX != 0xffffffff)
			#error NO UNSIGNED TYPE WITH DWORD SIZE WAS FOUND!
		#else
			typedef unsigned long uint32;
		#endif
	#else
		typedef unsigned int uint32;
	#endif

	#if(INT_MAX != 0x7fffffff)
		#if (LONG_MAX != 0x7fffffff)
			#error NO TYPE WITH DWORD SIZE WAS FOUND!
		#else
			typedef long int32;
		#endif
	#else
		typedef int int32;
	#endif


/* intH */
#endif