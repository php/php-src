/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/* Defines for Win32 to make it compatible for MySQL */

#include <sys/locking.h>
#include <windows.h>
#include <math.h>			/* Because of rint() */
#include <fcntl.h>
#include <io.h>
#include <malloc.h>

#if defined(__NT__)
#define	SYSTEM_TYPE	"NT"
#elif defined(__WIN2000__)
#define	SYSTEM_TYPE	"WIN2000"
#else
#define	SYSTEM_TYPE	"Win95/Win98"
#endif

#ifdef _WIN64
#define MACHINE_TYPE	"ia64"		/* Define to machine type name */
#else
#define MACHINE_TYPE	"i32"		/* Define to machine type name */
#ifndef _WIN32
#define _WIN32				/* Compatible with old source */
#endif
#ifndef __WIN32__
#define __WIN32__
#endif
#endif /* _WIN64 */
#ifndef __WIN__
#define __WIN__                       /* To make it easier in VC++ */
#endif

#define LONGLONG_MIN	((__int64) 0x8000000000000000)
#define LONGLONG_MAX	((__int64) 0x7FFFFFFFFFFFFFFF)
#define LL(A)		((__int64) A)

/* Type information */

/*
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned __int64 ulonglong;	
typedef __int64	longlong;
*/
typedef int sigset_t;
#define longlong_defined

#define SIZEOF_CHAR		1
#define SIZEOF_LONG		4
#define SIZEOF_LONG_LONG	8


#ifndef _WIN64
/* Optimized store functions for Intel x86 */

#define sint2korr(A)	(*((int16_t *) (A)))
#define sint3korr(A)	((int32_t) ((((uchar) (A)[2]) & 128) ? \
				  (((uint32_t) 255L << 24) | \
				   (((uint32_t) (uchar) (A)[2]) << 16) |\
				   (((uint32_t) (uchar) (A)[1]) << 8) | \
				   ((uint32_t) (uchar) (A)[0])) : \
				  (((uint32_t) (uchar) (A)[2]) << 16) |\
				  (((uint32_t) (uchar) (A)[1]) << 8) | \
				  ((uint32_t) (uchar) (A)[0])))
#define sint4korr(A)	(*((int32_t *) (A)))
#define uint2korr(A)	(*((uint16_t *) (A)))
#define uint3korr(A)	(int32_t) (*((uint32_t *) (A)) & 0xFFFFFF)
#define uint4korr(A)	(*((uint32_t *) (A)))
#define uint5korr(A)	((uint64_t)(((uint32_t) ((uchar) (A)[0])) +\
				    (((uint32_t) ((uchar) (A)[1])) << 8) +\
				    (((uint32_t) ((uchar) (A)[2])) << 16) +\
				    (((uint32_t) ((uchar) (A)[3])) << 24)) +\
			 	    (((uint64_t) ((uchar) (A)[4])) << 32))
#define uint8korr(A)	(*((uint64_t *) (A)))
#define sint8korr(A)	(*((int64_t *) (A)))
#define int2store(T,A)	*((uint16_t*) (T))= (uint16_t) (A)
#define int3store(T,A)		{ *(T)=  (uchar) ((A));\
				  *(T+1)=(uchar) (((uint32_t) (A) >> 8));\
				  *(T+2)=(uchar) (((A) >> 16)); }
#define int4store(T,A)	*((int32_t *) (T))= (int32_t) (A)
#define int5store(T,A)	{ *(T)= (uchar)((A));\
			  *((T)+1)=(uchar) (((A) >> 8));\
			  *((T)+2)=(uchar) (((A) >> 16));\
			  *((T)+3)=(uchar) (((A) >> 24)); \
			  *((T)+4)=(uchar) (((A) >> 32)); }
#define int8store(T,A)	*((uint64_t *) (T))= (uint64_t) (A)

#define doubleget(V,M)	{ *((int32_t *) &V) = *((int32_t*) M); \
			  *(((int32_t *) &V)+1) = *(((int32_t*) M)+1); }
#define doublestore(T,V) { *((int32_t *) T) = *((int32_t*) &V); \
			   *(((int32_t *) T)+1) = *(((int32_t*) &V)+1); }
#define float4get(V,M) { *((int32_t *) &(V)) = *((int32_t*) (M)); }
#define float8get(V,M) doubleget((V),(M))
#define float4store(V,M) memcpy((char*) V,(char*) (&M),sizeof(float))
#define float8store(V,M) doublestore((V),(M))

#endif /* _WIN64 */
