/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB
This file is public domain and comes with NO WARRANTY of any kind */

/* Defines for Win32 to make it compatible for MySQL */

#ifndef _MYSQLND_CONFIG_WIN_H
#define _MYSQLND_CONFIG_WIN_H

#include <sys/locking.h>
#include <windows.h>
#include <math.h>			/* Because of rint() */
#include <fcntl.h>
#include <io.h>
#include <malloc.h>

#include <stdint.h>

#ifndef HAVE_INT8_T
#define HAVE_INT8_T
#endif
#ifndef HAVE_UINT8_T
#define HAVE_UINT8_T
#endif
#ifndef HAVE_INT16_T
#define HAVE_INT16_T
#endif
#ifndef HAVE_UINT16_T
#define HAVE_UINT16_T
#endif
#ifndef HAVE_INT32_T
#define HAVE_INT32_T
#endif
#ifndef HAVE_UINT32_T
#define HAVE_UINT32_T
#endif
#ifndef HAVE_INT64_T
#define HAVE_INT64_T
#endif
#ifndef HAVE_UINT64_T
#define HAVE_UINT64_T
#endif


#ifndef _WIN64
#ifndef _WIN32
#define _WIN32				/* Compatible with old source */
#endif
#ifndef __WIN32__
#define __WIN32__
#endif
#endif /* _WIN64 */
#ifndef __WIN__
#define __WIN__				/* To make it easier in VC++ */
#endif

/* Type information */

#define SIZEOF_LONG		4
#define SIZEOF_LONG_LONG	8


#ifndef _WIN64
/* Optimized store functions for Intel x86 */

#define sint2korr(A)	(*((int16_t *) (A)))
#define sint3korr(A)	((int32_t) ((((zend_uchar) (A)[2]) & 128) ? \
										(((uint32_t) 255L << 24) | \
										(((uint32_t) (zend_uchar) (A)[2]) << 16) |\
										(((uint32_t) (zend_uchar) (A)[1]) << 8) | \
										((uint32_t) (zend_uchar) (A)[0])) : \
										(((uint32_t) (zend_uchar) (A)[2]) << 16) |\
										(((uint32_t) (zend_uchar) (A)[1]) << 8) | \
										((uint32_t) (zend_uchar) (A)[0])))
#define sint4korr(A)	(*((int32_t *) (A)))
#define uint2korr(A)	(*((uint16_t *) (A)))
#define uint3korr(A)	(int32_t) (*((uint32_t *) (A)) & 0xFFFFFF)
#define uint4korr(A)	(*((uint32_t *) (A)))
#define uint5korr(A)	((uint64_t)(((uint32_t) ((zend_uchar) (A)[0])) +\
									(((uint32_t) ((zend_uchar) (A)[1])) << 8) +\
									(((uint32_t) ((zend_uchar) (A)[2])) << 16) +\
									(((uint32_t) ((zend_uchar) (A)[3])) << 24)) +\
									(((uint64_t) ((zend_uchar) (A)[4])) << 32))
#define uint8korr(A)	(*((uint64_t *) (A)))
#define sint8korr(A)	(*((int64_t *) (A)))
#define int2store(T,A)	*((uint16_t*) (T))= (uint16_t) (A)
#define int3store(T,A)		{	*(T)=  (zend_uchar) ((A));\
								*(T+1)=(zend_uchar) (((uint32_t) (A) >> 8));\
								*(T+2)=(zend_uchar) (((A) >> 16)); }
#define int4store(T,A)	*((int32_t *) (T))= (int32_t) (A)
#define int5store(T,A)	{	*(T)= (zend_uchar)((A));\
							*((T)+1)=(zend_uchar) (((A) >> 8));\
							*((T)+2)=(zend_uchar) (((A) >> 16));\
							*((T)+3)=(zend_uchar) (((A) >> 24)); \
							*((T)+4)=(zend_uchar) (((A) >> 32)); }
#define int8store(T,A)	*((uint64_t *) (T))= (uint64_t) (A)

#define float8get(V,M)	{	*((int32_t *) &V) = *((int32_t*) M); \
							*(((int32_t *) &V)+1) = *(((int32_t*) M)+1); }
#define float8store(T,V) {	*((int32_t *) T) = *((int32_t*) &V); \
							*(((int32_t *) T)+1) = *(((int32_t*) &V)+1); }
#define float4get(V,M) { *((int32_t *) &(V)) = *((int32_t*) (M)); }

#endif /* _WIN64 */

#endif /* _MYSQLND_CONFIG_WIN_H */
