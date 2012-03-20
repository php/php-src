/*
 * FILE:	sha2.h
 * AUTHOR:	Aaron D. Gifford - http://www.aarongifford.com/
 *
 * Copyright (c) 2000-2001, Aaron D. Gifford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTOR(S) ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $OrigId: sha2.h,v 1.1 2001/11/08 00:02:01 adg Exp adg $
 * $RoughId: sha2.h,v 1.3 2002/02/24 08:14:32 knu Exp $
 * $Id$
 */

#ifndef __SHA2_H__
#define __SHA2_H__

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Import u_intXX_t size_t type definitions from system headers.  You
 * may need to change this, or define these things yourself in this
 * file.
 */
#include <sys/types.h>

#ifdef RUBY
# ifdef HAVE_PROTOTYPES
#  undef NOPROTO
# else
#  define NOPROTO
# endif /* HAVE_PROTOTYPES */
# ifndef BYTE_ORDER
#  define LITTLE_ENDIAN	1234
#  define BIG_ENDIAN	4321
#  ifdef WORDS_BIGENDIAN
#   define BYTE_ORDER	BIG_ENDIAN
#  else
#   define BYTE_ORDER	LITTLE_ENDIAN
#  endif
# endif /* BYTE_ORDER */
# define SHA2_USE_INTTYPES_H
#else /* RUBY */
#ifdef SHA2_USE_INTTYPES_H

#include <inttypes.h>

#endif /* SHA2_USE_INTTYPES_H */
#endif /* RUBY */


/*** SHA-256/384/512 Various Length Definitions ***********************/
#define SHA256_BLOCK_LENGTH		64
#define SHA256_DIGEST_LENGTH		32
#define SHA256_DIGEST_STRING_LENGTH	(SHA256_DIGEST_LENGTH * 2 + 1)
#define SHA384_BLOCK_LENGTH		128
#define SHA384_DIGEST_LENGTH		48
#define SHA384_DIGEST_STRING_LENGTH	(SHA384_DIGEST_LENGTH * 2 + 1)
#define SHA512_BLOCK_LENGTH		128
#define SHA512_DIGEST_LENGTH		64
#define SHA512_DIGEST_STRING_LENGTH	(SHA512_DIGEST_LENGTH * 2 + 1)


/*** SHA-256/384/512 Context Structures *******************************/
/* NOTE: If your architecture does not define either u_intXX_t types or
 * uintXX_t (from inttypes.h), you may need to define things by hand
 * for your system:
 */
#ifndef SHA2_USE_INTTYPES_H
# ifdef HAVE_U_INT8_T
typedef u_int8_t uint8_t;		/* 1-byte  (8-bits)  */
typedef u_int32_t uint32_t;		/* 4-bytes (32-bits) */
typedef u_int64_t uint64_t;		/* 8-bytes (64-bits) */
# else
typedef unsigned char uint8_t;		/* 1-byte  (8-bits)  */
typedef unsigned int uint32_t;		/* 4-bytes (32-bits) */
typedef unsigned long long uint64_t;	/* 8-bytes (64-bits) */
# endif
#endif

/*
 * Most BSD systems already define u_intXX_t types, as does Linux.
 * Some systems, however, like Compaq's Tru64 Unix instead can use
 * uintXX_t types defined by very recent ANSI C standards and included
 * in the file:
 *
 *   #include <inttypes.h>
 *
 * If you choose to use <inttypes.h> then please define:
 *
 *   #define SHA2_USE_INTTYPES_H
 *
 * Or on the command line during compile:
 *
 *   cc -DSHA2_USE_INTTYPES_H ...
 */
typedef struct _SHA256_CTX {
	uint32_t	state[8];
	uint64_t	bitcount;
	uint8_t	buffer[SHA256_BLOCK_LENGTH];
} SHA256_CTX;
typedef struct _SHA512_CTX {
	uint64_t	state[8];
	uint64_t	bitcount[2];
	uint8_t	buffer[SHA512_BLOCK_LENGTH];
} SHA512_CTX;

typedef SHA512_CTX SHA384_CTX;


/*** SHA-256/384/512 Function Prototypes ******************************/
#ifdef RUBY
#define SHA256_Init		rb_Digest_SHA256_Init
#define SHA256_Update		rb_Digest_SHA256_Update
#define SHA256_Finish		rb_Digest_SHA256_Finish
#define SHA256_Data		rb_Digest_SHA256_Data
#define SHA256_End		rb_Digest_SHA256_End
#define SHA256_Last		rb_Digest_SHA256_Last
#define SHA256_Transform	rb_Digest_SHA256_Transform
#define SHA256_Final(d, c)	SHA256_Finish(c, d)

#define SHA384_Init		rb_Digest_SHA384_Init
#define SHA384_Update		rb_Digest_SHA384_Update
#define SHA384_Finish		rb_Digest_SHA384_Finish
#define SHA384_Data		rb_Digest_SHA384_Data
#define SHA384_End		rb_Digest_SHA384_End
#define SHA384_Last		rb_Digest_SHA384_Last
#define SHA384_Transform	rb_Digest_SHA384_Transform
#define SHA384_Final(d, c)	SHA384_Finish(c, d)

#define SHA512_Init		rb_Digest_SHA512_Init
#define SHA512_Update		rb_Digest_SHA512_Update
#define SHA512_Finish		rb_Digest_SHA512_Finish
#define SHA512_Data		rb_Digest_SHA512_Data
#define SHA512_End		rb_Digest_SHA512_End
#define SHA512_Last		rb_Digest_SHA512_Last
#define SHA512_Transform	rb_Digest_SHA512_Transform
#define SHA512_Final(d, c)	SHA512_Finish(c, d)
#endif /* RUBY */

#ifndef NOPROTO

void SHA256_Init(SHA256_CTX *);
void SHA256_Update(SHA256_CTX*, const uint8_t*, size_t);
void SHA256_Final(uint8_t[SHA256_DIGEST_LENGTH], SHA256_CTX*);
char* SHA256_End(SHA256_CTX*, char[SHA256_DIGEST_STRING_LENGTH]);
char* SHA256_Data(const uint8_t*, size_t, char[SHA256_DIGEST_STRING_LENGTH]);

void SHA384_Init(SHA384_CTX*);
void SHA384_Update(SHA384_CTX*, const uint8_t*, size_t);
void SHA384_Final(uint8_t[SHA384_DIGEST_LENGTH], SHA384_CTX*);
char* SHA384_End(SHA384_CTX*, char[SHA384_DIGEST_STRING_LENGTH]);
char* SHA384_Data(const uint8_t*, size_t, char[SHA384_DIGEST_STRING_LENGTH]);

void SHA512_Init(SHA512_CTX*);
void SHA512_Update(SHA512_CTX*, const uint8_t*, size_t);
void SHA512_Final(uint8_t[SHA512_DIGEST_LENGTH], SHA512_CTX*);
char* SHA512_End(SHA512_CTX*, char[SHA512_DIGEST_STRING_LENGTH]);
char* SHA512_Data(const uint8_t*, size_t, char[SHA512_DIGEST_STRING_LENGTH]);

#else /* NOPROTO */

void SHA256_Init();
void SHA256_Update();
#ifdef RUBY
void SHA256_Finish();
#else
void SHA256_Final();
#endif /* RUBY */
char* SHA256_End();
char* SHA256_Data();

void SHA384_Init();
void SHA384_Update();
#ifdef RUBY
void SHA384_Finish();
#else
void SHA384_Final();
#endif /* RUBY */
char* SHA384_End();
char* SHA384_Data();

void SHA512_Init();
void SHA512_Update();
#ifdef RUBY
void SHA512_Finish();
#else
void SHA512_Final();
#endif /* RUBY */
char* SHA512_End();
char* SHA512_Data();

#endif /* NOPROTO */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* __SHA2_H__ */

