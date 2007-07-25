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

typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef unsigned __int64 ulonglong;	/* Microsofts 64 bit types */
typedef __int64	longlong;
typedef int sigset_t;
#define longlong_defined

#define SIZEOF_CHAR		1
#define SIZEOF_LONG		4
#define SIZEOF_LONG_LONG	8


#ifndef _WIN64
/* Optimized store functions for Intel x86 */

#define sint2korr(A)	(*((int16 *) (A)))
#define sint3korr(A)	((int32) ((((uchar) (A)[2]) & 128) ? \
				  (((uint32) 255L << 24) | \
				   (((uint32) (uchar) (A)[2]) << 16) |\
				   (((uint32) (uchar) (A)[1]) << 8) | \
				   ((uint32) (uchar) (A)[0])) : \
				  (((uint32) (uchar) (A)[2]) << 16) |\
				  (((uint32) (uchar) (A)[1]) << 8) | \
				  ((uint32) (uchar) (A)[0])))
#define sint4korr(A)	(*((long *) (A)))
#define uint2korr(A)	(*((uint16 *) (A)))
#define uint3korr(A)	(long) (*((unsigned long *) (A)) & 0xFFFFFF)
#define uint4korr(A)	(*((unsigned long *) (A)))
#define uint5korr(A)	((ulonglong)(((uint32) ((uchar) (A)[0])) +\
				    (((uint32) ((uchar) (A)[1])) << 8) +\
				    (((uint32) ((uchar) (A)[2])) << 16) +\
				    (((uint32) ((uchar) (A)[3])) << 24)) +\
			 	    (((ulonglong) ((uchar) (A)[4])) << 32))
#define uint8korr(A)	(*((ulonglong *) (A)))
#define sint8korr(A)	(*((longlong *) (A)))
#define int2store(T,A)	*((uint16*) (T))= (uint16) (A)
#define int3store(T,A)		{ *(T)=  (uchar) ((A));\
				  *(T+1)=(uchar) (((uint) (A) >> 8));\
				  *(T+2)=(uchar) (((A) >> 16)); }
#define int4store(T,A)	*((long *) (T))= (long) (A)
#define int5store(T,A)	{ *(T)= (uchar)((A));\
			  *((T)+1)=(uchar) (((A) >> 8));\
			  *((T)+2)=(uchar) (((A) >> 16));\
			  *((T)+3)=(uchar) (((A) >> 24)); \
			  *((T)+4)=(uchar) (((A) >> 32)); }
#define int8store(T,A)	*((ulonglong *) (T))= (ulonglong) (A)

#define doubleget(V,M)	{ *((long *) &V) = *((long*) M); \
			  *(((long *) &V)+1) = *(((long*) M)+1); }
#define doublestore(T,V) { *((long *) T) = *((long*) &V); \
			   *(((long *) T)+1) = *(((long*) &V)+1); }
#define float4get(V,M) { *((long *) &(V)) = *((long*) (M)); }
#define float8get(V,M) doubleget((V),(M))
#define float4store(V,M) memcpy((byte*) V,(byte*) (&M),sizeof(float))
#define float8store(V,M) doublestore((V),(M))

#endif /* _WIN64 */
