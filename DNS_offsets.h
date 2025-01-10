#ifndef included_DNS_offsets_h
#define included_DNS_offsets_h

/* used info from: https://mislove.org/teaching/cs4700/spring11/handouts/project1-primer.pdf */


/* DNS HEADER */

#define DNS_HEADER_OFFSET 0

#define DNS_HEADER_ID_OFFSET 0
#define DNS_HEADER_FLAGS_OFFSET 2
#define DNS_HEADER_QDCOUNT_OFFSET 4
#define DNS_HEADER_ANCOUNT_OFFSET 6
#define DNS_HEADER_MSCOUNT_OFFSET 8
#define DNS_HEADER_ARCOUNT_OFFSET 10

#define DNS_QR_BITOFFSET 15
#define DNS_OPCODE_BITOFFSET 11 
#define DNS_AA_BITOFFSET 10
#define DNS_TC_BITOFFSET 9
#define DNS_RD_BITOFFSET 8
#define DNS_RA_BITOFFSET 7
#define DNS_Z_BITOFFSET  4
#define DNS_RCODE_BITOFFSET 0


#define DNS_QUESTION_OFFSET 12

#define DNS_construct_flags(upper, lower) ((lower & 0xFF) | ((upper & 0xFF) << 8))

/*
hex values in bin
0b0111111111111111
0b1000011111111111
0b1111101111111111
0b1111110111111111
0b1111111011111111
0b1111111101111111
0b1111111110001111
0b1111111111110000
*/
#define DNS_FLAGS_SET_QR(flags, value) flags = 		(flags & 0x7FFF) | ((value & 0x1) << DNS_QR_BITOFFSET)
#define DNS_FLAGS_SET_OPCODE(flags, value) flags = 	(flags & 0x87FF) | ((value & 0xF) << DNS_OPCODE_BITOFFSET)
#define DNS_FLAGS_SET_AA(flags, value) flags = 		(flags & 0xFBFF) | ((value & 0x1) << DNS_AA_BITOFFSET)
#define DNS_FLAGS_SET_TC(flags, value) flags = 		(flags & 0xFDFF) | ((value & 0x1) << DNS_TC_BITOFFSET)
#define DNS_FLAGS_SET_RD(flags, value) flags = 		(flags & 0xFEFF) | ((value & 0x1) << DNS_RD_BITOFFSET)
#define DNS_FLAGS_SET_RA(flags, value) flags = 		(flags & 0xFF7F) | ((value & 0x1) << DNS_RA_BITOFFSET)
#define DNS_FLAGS_SET_Z(flags, value)  flags = 		(flags & 0xFF8F) | ((value & 0x7) << DNS_Z_BITOFFSET)
#define DNS_FLAGS_SET_RCODE(flags, value) flags = 	(flags & 0xFFF0) | ((value & 0xF) << DNS_RCODE_BITOFFSET)

/*
hex values in bin
0b1000000000000000
0b0111100000000000
0b0000010000000000
0b0000001000000000
0b0000000100000000
0b0000000010000000
0b0000000001110000
0b0000000000001111
*/
#define DNS_FLAGS_GET_QR(flags) 	((flags & 0x8000) >> DNS_QR_BITOFFSET)
#define DNS_FLAGS_GET_OPCODE(flags)	((flags & 0x7800) >> DNS_OPCODE_BITOFFSET)
#define DNS_FLAGS_GET_AA(flags)		((flags & 0x0400) >> DNS_AA_BITOFFSET)
#define DNS_FLAGS_GET_TC(flags)		((flags & 0x0200) >> DNS_TC_BITOFFSET)
#define DNS_FLAGS_GET_RD(flags)		((flags & 0x0100) >> DNS_RD_BITOFFSET)
#define DNS_FLAGS_GET_RA(flags)		((flags & 0x0080) >> DNS_RA_BITOFFSET)
#define DNS_FLAGS_GET_Z(flags)		((flags & 0x0070) >> DNS_Z_BITOFFSET)
#define DNS_FLAGS_GET_RCODE(flags) 	((flags & 0x000F) >> DNS_RCODE_BITOFFSET)

/* included_DNS_offsets_h */
#endif