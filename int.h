#ifndef intH
#define intH

/*
	This file defines these types:
	uint8, uint16, uint32,
	int8, int16, int32

	this is because ISO C does not have stdint.h

	if you have defined BITS64, then these types are also defined;
	uint64, int64
*/

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
		typedef signed char int8;
	#endif



	/* (u)int16 */ 
	#if(USHRT_MAX != 0xffff)
		#error UNSIGNED SHORT IS NOT 2 BYTES!
	#else
		typedef unsigned short uint16;
	#endif

	#if(SHRT_MAX != 0x7fff)
		#error SHORT IS NOT A 2 BYTES!
	#else
		typedef signed short int16;
	#endif


	/* (u)int32 */ 
	#if(UINT_MAX != 0xffffffff)
		#if (ULONG_MAX != 0xffffffff)
			#error NO UNSIGNED TYPE WITH SIZE OF 4 BYTES WAS FOUND!
		#else
			typedef unsigned long uint32;
		#endif
	#else
		typedef unsigned int uint32;
	#endif

	#if(INT_MAX != 0x7fffffff)
		#if (LONG_MAX != 0x7fffffff)
			#error NO UNSIGNED TYPE WITH SIZE OF 4 BYTES WAS FOUND!
		#else
			typedef signed long int32;
		#endif
	#else
		typedef signed int int32;
	#endif




	#ifdef BITS64

		/* (u)int64 */ 
		#if(UINT_MAX != 0xffffffffffffffff)
			#if (ULONG_MAX != 0xffffffffffffffff)

				#ifndef __SIZEOF_LONG_LONG__
					#error NO UNSIGNED TYPE WITH SIZE OF 8 BYTES WAS FOUND!
				#else
					#if (__SIZEOF_LONG_LONG__ != 0x8) /* has 8 bytes?*/
						#error NO UNSIGNED TYPE WITH SIZE OF 8 BYTES WAS FOUND!
					#else
						typedef unsigned long long uint64;
					#endif
				#endif

			#else
				typedef unsigned long uint64;
			#endif
		#else
			typedef unsigned int uint64;
		#endif


		#if(INT_MAX != 0x7fffffff)
			#if (LONG_MAX != 0x7fffffff)
				
				#ifndef __SIZEOF_LONG_LONG__
					#error NO TYPE WITH SIZE OF 8 BYTES WAS FOUND!
				#else
					#if (__SIZEOF_LONG_LONG__ != 0x8) /* has 8 bytes?*/
						#error NO TYPE WITH SIZE OF 8 BYTES WAS FOUND!
					#else
						typedef long long uint64;
					#endif
				#endif

			#else
				typedef long int64;
			#endif
		#else
			typedef int int64;
		#endif

	/* BITS64 */
	#endif


/* intH */
#endif

