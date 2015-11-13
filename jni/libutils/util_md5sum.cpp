/**
 * @file   util_md5sum.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:13:04 2010
 * 
 * @brief  
 * 
 * md5sum.c - Compute MD5 checksum of files or strings according to the
 *            definition of MD5 in RFC 1321 from April 1992.
 *
 * Copyright (C) 1995-1999 Free Software Foundation, Inc.
 * Written by Ulrich Drepper <drepper@gnu.ai.mit.edu>, 1995.
 *
 *
 * June 29, 2001        Manuel Novoa III
 *
 * Added MD5SUM_SIZE_VS_SPEED configuration option.
 *
 * Current valid values, with data from my system for comparison, are:
 *   (using uClibc and running on linux-2.4.4.tar.bz2)
 *                     user times (sec)  text size (386)
 *     0 (fastest)         1.1                6144
 *     1                   1.4                5392
 *     2                   3.0                5088
 *     3 (smallest)        5.1                4912
 */

#include <byteswap.h>
#include <endian.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "util_md5sum.h"

# define MD5SUM_SIZE_VS_SPEED 2

/* Handle endian-ness */
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define SWAP(n) (n)
# elif defined(bswap_32)
#  define SWAP(n) bswap_32(n)
# else
#  define SWAP(n) ((n << 24) | ((n&65280)<<8) | ((n&16711680)>>8) | (n>>24))
# endif

# if MD5SUM_SIZE_VS_SPEED == 0
/* This array contains the bytes used to pad the buffer to the next
   64-byte boundary.  (RFC 1321, 3.1: Step 1)  */
static const unsigned char fillbuf[64] = { 0x80, 0 /* , 0, 0, ...  */  };
# endif	/* MD5SUM_SIZE_VS_SPEED == 0 */

/* These are the four functions used in the four steps of the MD5 algorithm
 * and defined in the RFC 1321.  The first function is a little bit optimized
 * (as found in Colin Plumbs public domain implementation).
 * #define FF(b, c, d) ((b & c) | (~b & d))
 */
# define FF(b, c, d) (d ^ (b & (c ^ d)))
# define FG(b, c, d) FF (d, b, c)
# define FH(b, c, d) (b ^ c ^ d)
# define FI(b, c, d) (c ^ (b | ~d))

UtilMd5::UtilMd5 ()
{
    Begin ();
}

string UtilMd5::GetMd5String ()
{
    return mMd5string;
}
int UtilMd5::GetMd5BinBuffer (unsigned char* buf)
{
    if (buf != NULL)
    {
        memcpy (buf, mMd5Buf, sizeof (mMd5Buf));
        return sizeof (mMd5Buf);
    }
    return 0;
}

/* Initialize structure containing state of computation.
 * (RFC 1321, 3.3: Step 3)
 */
void UtilMd5::Begin ()
{
    muA = 0x67452301;
    muB = 0xefcdab89;
    muC = 0x98badcfe;
    muD = 0x10325476;
    muTotal[0] = muTotal[1] = 0;
    miBuflen = 0;
    memset (mBuffer, 0, sizeof (mBuffer));

    memset (mMd5Buf, 0, sizeof (mMd5Buf));
    mMd5string = "";
}

void UtilMd5::Hash (const void *buffer, size_t length)
{
	//if (length % 64 == 0) {
	//	md5_hash_block(buffer, length, md5_ctx);
	//} else {
    md5_hash_bytes(buffer, length);
    //}
}

/* Process the remaining bytes in the buffer and put result from CTX
 * in first 16 bytes following RESBUF.  The result is always in little
 * endian byte order, so that a byte-wise output yields to the wanted
 * ASCII representation of the message digest.
 *
 * IMPORTANT: On some systems it is required that RESBUF is correctly
 * aligned for a 32 bits value.
 */
void UtilMd5::End ()
{
	/* Take yet unprocessed bytes into account.  */
	int bytes = miBuflen;
	size_t pad;

	/* Now count remaining bytes.  */
	muTotal[0] += bytes;
	if (muTotal[0] < bytes)
		++muTotal[1];

	pad = bytes >= 56 ? 64 + 56 - bytes : 56 - bytes;
# if MD5SUM_SIZE_VS_SPEED > 0
	memset(&mBuffer[bytes], 0, pad);
    mBuffer[bytes] = 0x80;
# else
	memcpy(&mBuffer[bytes], fillbuf, pad);
# endif	/* MD5SUM_SIZE_VS_SPEED > 0 */

	/* Put the 64-bit file length in *bits* at the end of the buffer.  */
	*(U32 *) & mBuffer[bytes + pad] = SWAP(muTotal[0] << 3);
	*(U32 *) & mBuffer[bytes + pad + 4] =
		SWAP(((muTotal[1] << 3) | (muTotal[0] >> 29)));

	/* Process last bytes.  */
	md5_hash_block(mBuffer, bytes + pad + 8);

	/* Put result from CTX in first 16 bytes following RESBUF.  The result is
	 * always in little endian byte order, so that a byte-wise output yields
	 * to the wanted ASCII representation of the message digest.
	 *
	 * IMPORTANT: On some systems it is required that RESBUF is correctly
	 * aligned for a 32 bits value.
	 */
	((U32 *) mMd5Buf)[0] = SWAP(muA);
	((U32 *) mMd5Buf)[1] = SWAP(muB);
	((U32 *) mMd5Buf)[2] = SWAP(muC);
	((U32 *) mMd5Buf)[3] = SWAP(muD);

    mMd5string = Bin2Hex (mMd5Buf, sizeof (mMd5Buf));
	//return resbuf;
}

/* Starting with the result of former calls of this function (or the
 * initialization function update the context for the next LEN bytes
 * starting at BUFFER.
 * It is NOT required that LEN is a multiple of 64.
 */
void UtilMd5::md5_hash_bytes(const void *buffer, size_t len)
{
	/* When we already have some bits in our internal buffer concatenate
	   both inputs first.  */
	if (miBuflen != 0) {
		size_t left_over = miBuflen;
		size_t add = 128 - left_over > len ? len : 128 - left_over;

		memcpy(&mBuffer[left_over], buffer, add);
		miBuflen += add;

		if (left_over + add > 64) {
			md5_hash_block(mBuffer, (left_over + add) & ~63);
			/* The regions in the following copy operation cannot overlap.  */
			memcpy(mBuffer, &mBuffer[(left_over + add) & ~63],
				   (left_over + add) & 63);
			miBuflen = (left_over + add) & 63;
		}

		buffer = (const char *) buffer + add;
		len -= add;
	}

	/* Process available complete blocks.  */
	if (len > 64) {
		md5_hash_block(buffer, len & ~63);
		buffer = (const char *) buffer + (len & ~63);
		len &= 63;
	}

	/* Move remaining bytes in internal buffer.  */
	if (len > 0) {
		memcpy(mBuffer, buffer, len);
		miBuflen = len;
	}
}

/* Starting with the result of former calls of this function (or the
 * initialization function update the context for the next LEN bytes
 * starting at BUFFER.
 * It is necessary that LEN is a multiple of 64!!!
 */
void UtilMd5::md5_hash_block(const void *buffer, size_t len)
{
	U32 correct_words[16];
	const U32 *words = (U32*)buffer;
	size_t nwords = len / sizeof(U32);
	const U32 *endp = words + nwords;

# if MD5SUM_SIZE_VS_SPEED > 0
	static const U32 C_array[] = {
		/* round 1 */
		0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
		0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
		0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
		0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
		/* round 2 */
		0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
		0xd62f105d, 0x2441453, 0xd8a1e681, 0xe7d3fbc8,
		0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
		0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
		/* round 3 */
		0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
		0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
		0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x4881d05,
		0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
		/* round 4 */
		0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
		0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
		0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
		0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
	};

	static const char P_array[] = {
#  if MD5SUM_SIZE_VS_SPEED > 1
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,	/* 1 */
#  endif	/* MD5SUM_SIZE_VS_SPEED > 1 */
		1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12,	/* 2 */
		5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2,	/* 3 */
		0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9	/* 4 */
	};

#  if MD5SUM_SIZE_VS_SPEED > 1
	static const char S_array[] = {
		7, 12, 17, 22,
		5, 9, 14, 20,
		4, 11, 16, 23,
		6, 10, 15, 21
	};
#  endif	/* MD5SUM_SIZE_VS_SPEED > 1 */
# endif

	U32 A = muA;
	U32 B = muB;
	U32 C = muC;
	U32 D = muD;

	/* First increment the byte count.  RFC 1321 specifies the possible
	   length of the file up to 2^64 bits.  Here we only compute the
	   number of bytes.  Do a double word increment.  */
	muTotal[0] += len;
	if (muTotal[0] < len)
		++muTotal[1];

	/* Process all bytes in the buffer with 64 bytes in each round of
	   the loop.  */
	while (words < endp) {
		U32 *cwp = correct_words;
		U32 A_save = A;
		U32 B_save = B;
		U32 C_save = C;
		U32 D_save = D;

# if MD5SUM_SIZE_VS_SPEED > 1
#  define CYCLIC(w, s) (w = (w << s) | (w >> (32 - s)))

		const U32 *pc;
		const char *pp;
		const char *ps;
		int i;
		U32 temp;

		for (i = 0; i < 16; i++) {
			cwp[i] = SWAP(words[i]);
		}
		words += 16;

#  if MD5SUM_SIZE_VS_SPEED > 2
		pc = C_array;
		pp = P_array;
		ps = S_array - 4;

		for (i = 0; i < 64; i++) {
			if ((i & 0x0f) == 0)
				ps += 4;
			temp = A;
			switch (i >> 4) {
			case 0:
				temp += FF(B, C, D);
				break;
			case 1:
				temp += FG(B, C, D);
				break;
			case 2:
				temp += FH(B, C, D);
				break;
			case 3:
				temp += FI(B, C, D);
			}
			temp += cwp[(int) (*pp++)] + *pc++;
			CYCLIC(temp, ps[i & 3]);
			temp += B;
			A = D;
			D = C;
			C = B;
			B = temp;
		}
#  else
		pc = C_array;
		pp = P_array;
		ps = S_array;

		for (i = 0; i < 16; i++) {
			temp = A + FF(B, C, D) + cwp[(int) (*pp++)] + *pc++;
			CYCLIC(temp, ps[i & 3]);
			temp += B;
			A = D;
			D = C;
			C = B;
			B = temp;
		}

		ps += 4;
		for (i = 0; i < 16; i++) {
			temp = A + FG(B, C, D) + cwp[(int) (*pp++)] + *pc++;
			CYCLIC(temp, ps[i & 3]);
			temp += B;
			A = D;
			D = C;
			C = B;
			B = temp;
		}
		ps += 4;
		for (i = 0; i < 16; i++) {
			temp = A + FH(B, C, D) + cwp[(int) (*pp++)] + *pc++;
			CYCLIC(temp, ps[i & 3]);
			temp += B;
			A = D;
			D = C;
			C = B;
			B = temp;
		}
		ps += 4;
		for (i = 0; i < 16; i++) {
			temp = A + FI(B, C, D) + cwp[(int) (*pp++)] + *pc++;
			CYCLIC(temp, ps[i & 3]);
			temp += B;
			A = D;
			D = C;
			C = B;
			B = temp;
		}

#  endif	/* MD5SUM_SIZE_VS_SPEED > 2 */
# else
		/* First round: using the given function, the context and a constant
		   the next context is computed.  Because the algorithms processing
		   unit is a 32-bit word and it is determined to work on words in
		   little endian byte order we perhaps have to change the byte order
		   before the computation.  To reduce the work for the next steps
		   we store the swapped words in the array CORRECT_WORDS.  */

#  define OP(a, b, c, d, s, T)	\
      do	\
        {	\
	  a += FF (b, c, d) + (*cwp++ = SWAP (*words)) + T;	\
	  ++words;	\
	  CYCLIC (a, s);	\
	  a += b;	\
        }	\
      while (0)

		/* It is unfortunate that C does not provide an operator for
		   cyclic rotation.  Hope the C compiler is smart enough.  */
		/* gcc 2.95.4 seems to be --aaronl */
#  define CYCLIC(w, s) (w = (w << s) | (w >> (32 - s)))

		/* Before we start, one word to the strange constants.
		   They are defined in RFC 1321 as

		   T[i] = (int) (4294967296.0 * fabs (sin (i))), i=1..64
		 */

#  if MD5SUM_SIZE_VS_SPEED == 1
		const U32 *pc;
		const char *pp;
		int i;
#  endif	/* MD5SUM_SIZE_VS_SPEED */

		/* Round 1.  */
#  if MD5SUM_SIZE_VS_SPEED == 1
		pc = C_array;
		for (i = 0; i < 4; i++) {
			OP(A, B, C, D, 7, *pc++);
			OP(D, A, B, C, 12, *pc++);
			OP(C, D, A, B, 17, *pc++);
			OP(B, C, D, A, 22, *pc++);
		}
#  else
		OP(A, B, C, D, 7, 0xd76aa478);
		OP(D, A, B, C, 12, 0xe8c7b756);
		OP(C, D, A, B, 17, 0x242070db);
		OP(B, C, D, A, 22, 0xc1bdceee);
		OP(A, B, C, D, 7, 0xf57c0faf);
		OP(D, A, B, C, 12, 0x4787c62a);
		OP(C, D, A, B, 17, 0xa8304613);
		OP(B, C, D, A, 22, 0xfd469501);
		OP(A, B, C, D, 7, 0x698098d8);
		OP(D, A, B, C, 12, 0x8b44f7af);
		OP(C, D, A, B, 17, 0xffff5bb1);
		OP(B, C, D, A, 22, 0x895cd7be);
		OP(A, B, C, D, 7, 0x6b901122);
		OP(D, A, B, C, 12, 0xfd987193);
		OP(C, D, A, B, 17, 0xa679438e);
		OP(B, C, D, A, 22, 0x49b40821);
#  endif	/* MD5SUM_SIZE_VS_SPEED == 1 */

		/* For the second to fourth round we have the possibly swapped words
		   in CORRECT_WORDS.  Redefine the macro to take an additional first
		   argument specifying the function to use.  */
#  undef OP
#  define OP(f, a, b, c, d, k, s, T)	\
      do	\
	{	\
	  a += f (b, c, d) + correct_words[k] + T;	\
	  CYCLIC (a, s);	\
	  a += b;	\
	}	\
      while (0)

		/* Round 2.  */
#  if MD5SUM_SIZE_VS_SPEED == 1
		pp = P_array;
		for (i = 0; i < 4; i++) {
			OP(FG, A, B, C, D, (int) (*pp++), 5, *pc++);
			OP(FG, D, A, B, C, (int) (*pp++), 9, *pc++);
			OP(FG, C, D, A, B, (int) (*pp++), 14, *pc++);
			OP(FG, B, C, D, A, (int) (*pp++), 20, *pc++);
		}
#  else
		OP(FG, A, B, C, D, 1, 5, 0xf61e2562);
		OP(FG, D, A, B, C, 6, 9, 0xc040b340);
		OP(FG, C, D, A, B, 11, 14, 0x265e5a51);
		OP(FG, B, C, D, A, 0, 20, 0xe9b6c7aa);
		OP(FG, A, B, C, D, 5, 5, 0xd62f105d);
		OP(FG, D, A, B, C, 10, 9, 0x02441453);
		OP(FG, C, D, A, B, 15, 14, 0xd8a1e681);
		OP(FG, B, C, D, A, 4, 20, 0xe7d3fbc8);
		OP(FG, A, B, C, D, 9, 5, 0x21e1cde6);
		OP(FG, D, A, B, C, 14, 9, 0xc33707d6);
		OP(FG, C, D, A, B, 3, 14, 0xf4d50d87);
		OP(FG, B, C, D, A, 8, 20, 0x455a14ed);
		OP(FG, A, B, C, D, 13, 5, 0xa9e3e905);
		OP(FG, D, A, B, C, 2, 9, 0xfcefa3f8);
		OP(FG, C, D, A, B, 7, 14, 0x676f02d9);
		OP(FG, B, C, D, A, 12, 20, 0x8d2a4c8a);
#  endif	/* MD5SUM_SIZE_VS_SPEED == 1 */

		/* Round 3.  */
#  if MD5SUM_SIZE_VS_SPEED == 1
		for (i = 0; i < 4; i++) {
			OP(FH, A, B, C, D, (int) (*pp++), 4, *pc++);
			OP(FH, D, A, B, C, (int) (*pp++), 11, *pc++);
			OP(FH, C, D, A, B, (int) (*pp++), 16, *pc++);
			OP(FH, B, C, D, A, (int) (*pp++), 23, *pc++);
		}
#  else
		OP(FH, A, B, C, D, 5, 4, 0xfffa3942);
		OP(FH, D, A, B, C, 8, 11, 0x8771f681);
		OP(FH, C, D, A, B, 11, 16, 0x6d9d6122);
		OP(FH, B, C, D, A, 14, 23, 0xfde5380c);
		OP(FH, A, B, C, D, 1, 4, 0xa4beea44);
		OP(FH, D, A, B, C, 4, 11, 0x4bdecfa9);
		OP(FH, C, D, A, B, 7, 16, 0xf6bb4b60);
		OP(FH, B, C, D, A, 10, 23, 0xbebfbc70);
		OP(FH, A, B, C, D, 13, 4, 0x289b7ec6);
		OP(FH, D, A, B, C, 0, 11, 0xeaa127fa);
		OP(FH, C, D, A, B, 3, 16, 0xd4ef3085);
		OP(FH, B, C, D, A, 6, 23, 0x04881d05);
		OP(FH, A, B, C, D, 9, 4, 0xd9d4d039);
		OP(FH, D, A, B, C, 12, 11, 0xe6db99e5);
		OP(FH, C, D, A, B, 15, 16, 0x1fa27cf8);
		OP(FH, B, C, D, A, 2, 23, 0xc4ac5665);
#  endif	/* MD5SUM_SIZE_VS_SPEED == 1 */

		/* Round 4.  */
#  if MD5SUM_SIZE_VS_SPEED == 1
		for (i = 0; i < 4; i++) {
			OP(FI, A, B, C, D, (int) (*pp++), 6, *pc++);
			OP(FI, D, A, B, C, (int) (*pp++), 10, *pc++);
			OP(FI, C, D, A, B, (int) (*pp++), 15, *pc++);
			OP(FI, B, C, D, A, (int) (*pp++), 21, *pc++);
		}
#  else
		OP(FI, A, B, C, D, 0, 6, 0xf4292244);
		OP(FI, D, A, B, C, 7, 10, 0x432aff97);
		OP(FI, C, D, A, B, 14, 15, 0xab9423a7);
		OP(FI, B, C, D, A, 5, 21, 0xfc93a039);
		OP(FI, A, B, C, D, 12, 6, 0x655b59c3);
		OP(FI, D, A, B, C, 3, 10, 0x8f0ccc92);
		OP(FI, C, D, A, B, 10, 15, 0xffeff47d);
		OP(FI, B, C, D, A, 1, 21, 0x85845dd1);
		OP(FI, A, B, C, D, 8, 6, 0x6fa87e4f);
		OP(FI, D, A, B, C, 15, 10, 0xfe2ce6e0);
		OP(FI, C, D, A, B, 6, 15, 0xa3014314);
		OP(FI, B, C, D, A, 13, 21, 0x4e0811a1);
		OP(FI, A, B, C, D, 4, 6, 0xf7537e82);
		OP(FI, D, A, B, C, 11, 10, 0xbd3af235);
		OP(FI, C, D, A, B, 2, 15, 0x2ad7d2bb);
		OP(FI, B, C, D, A, 9, 21, 0xeb86d391);
#  endif	/* MD5SUM_SIZE_VS_SPEED == 1 */
# endif	/* MD5SUM_SIZE_VS_SPEED > 1 */

		/* Add the starting values of the context.  */
		A += A_save;
		B += B_save;
		C += C_save;
		D += D_save;
	}

	/* Put checksum in context given as argument.  */
	muA = A;
	muB = B;
	muC = C;
	muD = D;
}

/* This might be useful elsewhere */
string UtilMd5::Bin2Hex(unsigned char *hash_value, unsigned char hash_length)
{
	int x, len, max;
	unsigned char *hex_value;
    string hexHashValue;

	max = (hash_length * 2) + 2;
	hex_value = (unsigned char*)malloc (max);
    if(hex_value == NULL)
        return NULL;
    memset(hex_value, 0, max);

	for (x = len = 0; x < hash_length; x++) {
		len += snprintf((char *)hex_value + len, max - len, "%02x", hash_value[x]);
	}
    hexHashValue = (char*)hex_value;
    free (hex_value);

	return hexHashValue;
}

string UtilMd5::HashFd(int src_fd, const size_t size)
{
	int result = 0;
	size_t blocksize = 0;
	size_t remaining = size;
	unsigned char *buffer = NULL;
    UtilMd5* md5Ctx = new UtilMd5 ();
    string hexStr = "";

    blocksize = 4096;
    buffer = (unsigned char*)malloc (blocksize + 72);
    memset(buffer, 0, blocksize + 72);
    //cx = &md5_cx;
    
	/* Initialize the computation context.  */
    md5Ctx->Begin ();
    //md5_begin(&md5_cx);

	/* Iterate over full file contents.  */
	while ((remaining == (size_t) -1) || (remaining > 0)) {
		size_t read_try;
		ssize_t read_got;

		if (remaining > blocksize) {
			read_try = blocksize;
		} else {
			read_try = remaining;
		}
		//read_got = bb_full_read(src_fd, buffer, read_try);
        do {
            read_got = read (src_fd, buffer, read_try);
        } while (read_got < 0 && errno == EINTR);

		if (read_got < 1) {
			/* count == 0 means short read
			 * count == -1 means read error */
			result = read_got - 1;
			break;
		}
		if (remaining != (size_t) -1) {
			remaining -= read_got;
		}

		/* Process buffer */
        md5Ctx->Hash (buffer, read_got);
		//md5_hash(buffer, read_got, cx);
	}

	/* Finalize and write the hash into our buffer.  */
    md5Ctx->End ();
    hexStr = md5Ctx->GetMd5String ();
    //md5_end(hashval, &md5_cx);
    
	free (buffer);
    delete md5Ctx;
	return hexStr;
}

string UtilMd5::HashFile(const char *filename)
{
    string hash_value;
	int src_fd;

    src_fd = open (filename, O_RDONLY);

    if(src_fd == -1)
        return NULL;

	if (src_fd != -1) {
        hash_value = HashFd (src_fd, -1);
	} else {
		printf ("error:%s\n", filename);
	}

	close (src_fd);

	return hash_value;
}

string UtilMd5::HashBuffer(unsigned char *buf, const size_t len)
{
    UtilMd5 md5Ctx;
    string hexStr = "";

    md5Ctx.Begin ();
    md5Ctx.Hash (buf, len);
    md5Ctx.End ();
    hexStr = md5Ctx.GetMd5String ();

	return hexStr;
}

#ifdef MD5_MAIN
int main(int argc, char **argv)
{
	printf("md5:%s\n", hash_file(argv[1]).c_str ());
}
#endif
