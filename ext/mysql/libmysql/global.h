/* Copyright (C) 2000 MySQL AB & MySQL Finland AB & TCX DataKonsult AB
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA */

/* This is the main include file that should included 'first' in every
   C file. */

#ifndef _global_h
#define _global_h

#if defined(_WIN32) || defined(_WIN64)
#include <config-win.h>
#else
#include <my_config.h>
#endif
#if defined(__cplusplus)
#if defined(inline)
#undef inline				/* fix configure problem */
#endif
#endif /* _cplusplus */

/* The client defines this to avoid all thread code */
#if defined(UNDEF_THREADS_HACK)
#undef THREAD
#undef HAVE_mit_thread
#undef HAVE_LINUXTHREADS
#undef HAVE_UNIXWARE7_THREADS
#endif

#ifdef HAVE_THREADS_WITHOUT_SOCKETS
/* MIT pthreads does not work with unix sockets */
#undef HAVE_SYS_UN_H
#endif

#define __EXTENSIONS__ 1	/* We want some extension */
#ifndef __STDC_EXT__
#define __STDC_EXT__ 1          /* To get large file support on hpux */
#endif
#if defined(THREAD) && defined(HAVE_LINUXTHREADS) && defined(HAVE_PTHREAD_RWLOCK_RDLOCK)
#define _GNU_SOURCE 1
#endif

#if defined(THREAD) && !defined(__WIN__)
#define _POSIX_PTHREAD_SEMANTICS /* We want posix threads */
/* was #if defined(HAVE_LINUXTHREADS) || defined(HAVE_DEC_THREADS) || defined(HPUX) */
#if !defined(SCO)
#define _REENTRANT	1	/* Some thread libraries require this */
#endif
#if !defined(_THREAD_SAFE) && !defined(_AIX)
#define _THREAD_SAFE            /* Required for OSF1 */
#endif
#ifndef HAVE_mit_thread
#ifdef HAVE_UNIXWARE7_THREADS
#include <thread.h>
#else
#include <pthread.h>		/* AIX must have this included first */
#endif /* HAVE_UNIXWARE7_THREADS */
#endif /* HAVE_mit_thread */
#if !defined(SCO) && !defined(_REENTRANT)
#define _REENTRANT	1	/* Threads requires reentrant code */
#endif
#endif /* THREAD */

/* Go around some bugs in different OS and compilers */
#ifdef _AIX			/* By soren@t.dk */
#define _H_STRINGS
#define _SYS_STREAM_H
#define _AIX32_CURSES
#endif

#ifdef HAVE_BROKEN_SNPRINTF	/* HPUX 10.20 don't have this defined */
#undef HAVE_SNPRINTF
#endif
#ifdef HAVE_BROKEN_PREAD	/* These doesn't work on HPUX 11.x */
#undef HAVE_PREAD
#undef HAVE_PWRITE
#endif

#ifdef UNDEF_HAVE_GETHOSTBYNAME_R		/* For OSF4.x */
#undef HAVE_GETHOSTBYNAME_R
#endif
#ifdef UNDEF_HAVE_INITGROUPS			/* For AIX 4.3 */
#undef HAVE_INITGROUPS
#endif

/* Fix a bug in gcc 2.8.0 on IRIX 6.2 */
#if SIZEOF_LONG == 4 && defined(__LONG_MAX__)
#undef __LONG_MAX__             /* Is a longlong value in gcc 2.8.0 ??? */
#define __LONG_MAX__ 2147483647
#endif

/* egcs 1.1.2 has a problem with memcpy on Alpha */
#if defined(__GNUC__) && defined(__alpha__) && ! (__GNUC__ > 2 || (__GNUC__ == 2 &&  __GNUC_MINOR__ >= 95))
#define BAD_MEMCPY
#endif

/* In Linux-alpha we have atomic.h if we are using gcc */
#if defined(HAVE_LINUXTHREADS) && defined(__GNUC__) && defined(__alpha__) && (__GNUC__ > 2 || ( __GNUC__ == 2 &&  __GNUC_MINOR__ >= 95))
#define HAVE_ATOMIC_ADD
#define HAVE_ATOMIC_SUB
#endif

/* In Linux-ia64 including atomic.h will give us an error */
#if defined(HAVE_LINUXTHREADS) && defined(__GNUC__) && defined(__ia64__)
#undef HAVE_ATOMIC_ADD
#undef HAVE_ATOMIC_SUB
#endif

#if defined(_lint) && !defined(lint)
#define lint
#endif
#if SIZEOF_LONG_LONG > 4 && !defined(_LONG_LONG)
#define _LONG_LONG 1		/* For AIX string library */
#endif

#ifndef stdin
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include <math.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_FLOAT_H
#include <float.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_SYS_TIMEB_H
#include <sys/timeb.h>				/* Avoid warnings on SCO */
#endif
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif /* TIME_WITH_SYS_TIME */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#if defined(__cplusplus) && defined(NO_CPLUSPLUS_ALLOCA)
#undef HAVE_ALLOCA
#undef HAVE_ALLOCA_H
#endif
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#ifdef HAVE_ATOMIC_ADD
#define __SMP__
#include <asm/atomic.h>
#endif

/* Go around some bugs in different OS and compilers */
#if defined(_HPUX_SOURCE) && defined(HAVE_SYS_STREAM_H)
#include <sys/stream.h>		/* HPUX 10.20 defines ulong here. UGLY !!! */
#define HAVE_ULONG
#endif
#ifdef DONT_USE_FINITE		/* HPUX 11.x has is_finite() */
#undef HAVE_FINITE
#endif

/* We can not live without these */

#define USE_MYFUNC 1		/* Must use syscall indirection */
#define MASTER 1		/* Compile without unireg */
#define ENGLISH 1		/* Messages in English */
#define POSIX_MISTAKE 1		/* regexp: Fix stupid spec error */
#define USE_REGEX 1		/* We want the use the regex library */
/* Do not define for ultra sparcs */
#define USE_BMOVE512 1		/* Use this unless the system bmove is faster */

/* Paranoid settings. Define I_AM_PARANOID if you are paranoid */
#ifdef I_AM_PARANOID
#define DONT_ALLOW_USER_CHANGE 1
#define DONT_USE_MYSQL_PWD 1
#endif

/* #define USE_some_charset 1 was deprecated by changes to configure */
/* my_ctype my_to_upper, my_to_lower, my_sort_order gain theit right value */
/* automagically during configuration */

/* Does the system remember a signal handler after a signal ? */
#ifndef HAVE_BSD_SIGNALS
#define DONT_REMEMBER_SIGNAL
#endif

/* Define void to stop lint from generating "null effekt" comments */
#ifndef DONT_DEFINE_VOID
#ifdef _lint
int	__void__;
#define VOID(X)		(__void__ = (int) (X))
#else
#undef VOID
#define VOID(X)		(X)
#endif
#endif /* DONT_DEFINE_VOID */

#if defined(_lint) || defined(FORCE_INIT_OF_VARS)
#define LINT_INIT(var)	var=0			/* No uninitialize-warning */
#else
#define LINT_INIT(var)
#endif

/* Define som useful general macros */
#if defined(__cplusplus) && defined(__GNUC__)
#define max(a, b)	((a) >? (b))
#define min(a, b)	((a) <? (b))
#elif !defined(max)
#define max(a, b)	((a) > (b) ? (a) : (b))
#define min(a, b)	((a) < (b) ? (a) : (b))
#endif

#if defined(__EMX__) || !defined(HAVE_UINT)
typedef unsigned int uint;
typedef unsigned short ushort;
#endif

#define sgn(a)		(((a) < 0) ? -1 : ((a) > 0) ? 1 : 0)
#define swap(t,a,b)	{ register t dummy; dummy = a; a = b; b = dummy; }
#define test(a)		((a) ? 1 : 0)
#define set_if_bigger(a,b)  { if ((a) < (b)) (a)=(b); }
#define set_if_smaller(a,b) { if ((a) > (b)) (a)=(b); }
#define test_all_bits(a,b) (((a) & (b)) == (b))
#define array_elements(A) ((uint) (sizeof(A)/sizeof(A[0])))
#ifndef HAVE_RINT
#define rint(A) floor((A)+0.5)
#endif

/* Define som general constants */
#ifndef TRUE
#define TRUE		(1)	/* Logical true */
#define FALSE		(0)	/* Logical false */
#endif

#if defined(__GNUC__)
#define function_volatile	volatile
#define my_reinterpret_cast(A) reinterpret_cast<A>
#define my_const_cast(A) const_cast<A>
#elif !defined(my_reinterpret_cast)
#define my_reinterpret_cast(A) (A)
#define my_const_cast(A) (A)
#endif
#if !defined(__attribute__) && (defined(__cplusplus) || !defined(__GNUC__)  || __GNUC__ == 2 && __GNUC_MINOR__ < 8)
#define __attribute__(A)
#endif

/* From old s-system.h */

/* Support macros for non ansi & other old compilers. Since such
   things are no longer supported we do nothing. We keep then since
   some of our code may still be needed to upgrade old customers. */
#define _VARARGS(X) X
#define _STATIC_VARARGS(X) X
#define _PC(X)	X

#if defined(DBUG_ON) && defined(DBUG_OFF)
#undef DBUG_OFF
#endif

#if defined(_lint) && !defined(DBUG_OFF)
#define DBUG_OFF
#endif

#include <dbug.h>
#ifndef DBUG_OFF
#define dbug_assert(A) assert(A)
#else
#define dbug_assert(A)
#endif

#define MIN_ARRAY_SIZE	0	/* Zero or One. Gcc allows zero*/
#define ASCII_BITS_USED 8	/* Bit char used */
#define NEAR_F			/* No near function handling */

/* Some types that is different between systems */

typedef int	File;		/* File descriptor */
#ifndef Socket_defined
typedef int	my_socket;	/* File descriptor for sockets */
#define INVALID_SOCKET -1
#endif
typedef RETSIGTYPE sig_handler; /* Function to handle signals */
typedef void	(*sig_return)();/* Returns type from signal */
#if defined(__GNUC__) && !defined(_lint)
typedef char	pchar;		/* Mixed prototypes can take char */
typedef char	puchar;		/* Mixed prototypes can take char */
typedef char	pbool;		/* Mixed prototypes can take char */
typedef short	pshort;		/* Mixed prototypes can take short int */
typedef float	pfloat;		/* Mixed prototypes can take float */
#else
typedef int	pchar;		/* Mixed prototypes can't take char */
typedef uint	puchar;		/* Mixed prototypes can't take char */
typedef int	pbool;		/* Mixed prototypes can't take char */
typedef int	pshort;		/* Mixed prototypes can't take short int */
typedef double	pfloat;		/* Mixed prototypes can't take float */
#endif
typedef int	(*qsort_cmp)(const void *,const void *);
#ifdef HAVE_mit_thread
#define qsort_t void
#undef QSORT_TYPE_IS_VOID
#define QSORT_TYPE_IS_VOID
#else
#define qsort_t RETQSORTTYPE	/* Broken GCC cant handle typedef !!!! */
#endif
#ifdef HAVE_mit_thread
#define size_socket socklen_t	/* Type of last arg to accept */
#else
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
typedef SOCKET_SIZE_TYPE size_socket;
#endif

/* file create flags */

#ifndef O_SHARE
#define O_SHARE		0	/* Flag to my_open for shared files */
#ifndef O_BINARY
#define O_BINARY	0	/* Flag to my_open for binary files */
#endif
#define FILE_BINARY	0	/* Flag to my_fopen for binary streams */
#ifdef HAVE_FCNTL
#define HAVE_FCNTL_LOCK
#define F_TO_EOF	0L	/* Param to lockf() to lock rest of file */
#endif
#endif /* O_SHARE */
#ifndef O_TEMPORARY
#define O_TEMPORARY	0
#endif
#ifndef O_SHORT_LIVED
#define O_SHORT_LIVED	0
#endif

/* #define USE_RECORD_LOCK	*/

	/* Unsigned types supported by the compiler */
#define UNSINT8			/* unsigned int8 (char) */
#define UNSINT16		/* unsigned int16 */
#define UNSINT32		/* unsigned int32 */

	/* General constants */
#define SC_MAXWIDTH	256	/* Max width of screen (for error messages) */
#define FN_LEN		256	/* Max file name len */
#define FN_HEADLEN	253	/* Max length of filepart of file name */
#define FN_EXTLEN	20	/* Max length of extension (part of FN_LEN) */
#define FN_REFLEN	512	/* Max length of full path-name */
#define FN_EXTCHAR	'.'
#define FN_HOMELIB	'~'	/* ~/ is used as abbrev for home dir */
#define FN_CURLIB	'.'	/* ./ is used as abbrev for current dir */
#define FN_PARENTDIR	".."	/* Parentdirectory; Must be a string */
#define FN_DEVCHAR	':'

#ifndef FN_LIBCHAR
#define FN_LIBCHAR	'/'
#define FN_ROOTDIR	"/"
#define MY_NFILE	127	/* This is only used to save filenames */
#endif

/* #define EXT_IN_LIBNAME     */
/* #define FN_NO_CASE_SENCE   */
/* #define FN_UPPER_CASE TRUE */

/* Io buffer size; Must be a power of 2 and a multiple of 512. May be
   smaller what the disk page size. This influences the speed of the
   isam btree library. eg to big to slow. */
#define IO_SIZE			4096
/* How much overhead does malloc have. The code often allocates
   something like 1024-MALLOC_OVERHEAD bytes */
#ifdef SAFEMALLOC
#define MALLOC_OVERHEAD (8+24+4)
#else
#define MALLOC_OVERHEAD 8
#endif
	/* get memory in huncs */
#define ONCE_ALLOC_INIT		(uint) (4096-MALLOC_OVERHEAD)
	/* Typical record cash */
#define RECORD_CACHE_SIZE	(uint) (64*1024-MALLOC_OVERHEAD)
	/* Typical key cash */
#define KEY_CACHE_SIZE		(uint) (8*1024*1024-MALLOC_OVERHEAD)

	/* Some things that this system doesn't have */

#define ONLY_OWN_DATABASES	/* We are using only databases by monty */
#define NO_PISAM		/* Not needed anymore */
#define NO_MISAM		/* Not needed anymore */
#define NO_HASH			/* Not needed anymore */
#ifdef __WIN__
#define NO_DIR_LIBRARY		/* Not standar dir-library */
#define USE_MY_STAT_STRUCT	/* For my_lib */
#endif

/* Some things that this system does have */

#ifndef HAVE_ITOA
#define USE_MY_ITOA		/* There is no itoa */
#endif

/* Some defines of functions for portability */

#ifndef HAVE_ATOD
#define atod		atof
#endif
#ifdef USE_MY_ATOF
#define atof		my_atof
extern void		init_my_atof(void);
extern double		my_atof(const char*);
#endif
#undef remove		/* Crashes MySQL on SCO 5.0.0 */
#ifndef __WIN__
#define closesocket(A)	close(A)
#ifndef ulonglong2double
#define ulonglong2double(A) ((double) (A))
#define my_off_t2double(A)  ((double) (A))
#endif
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif
#define ulong_to_double(X) ((double) (ulong) (X))
#define SET_STACK_SIZE(X)	/* Not needed on real machines */

#if !defined(HAVE_mit_thread) && !defined(HAVE_STRTOK_R)
#define strtok_r(A,B,C) strtok((A),(B))
#endif

#ifdef HAVE_LINUXTHREADS
/* #define pthread_sigmask(A,B,C) sigprocmask((A),(B),(C)) */
/* #define sigset(A,B) signal((A),(B)) */
#endif

/* Remove some things that mit_thread break or doesn't support */
#if defined(HAVE_mit_thread) && defined(THREAD)
#undef HAVE_PREAD
#undef HAVE_REALPATH
#undef HAVE_MLOCK
#undef HAVE_TEMPNAM				/* Use ours */
#undef HAVE_PTHREAD_SETPRIO
#endif

/* This is from the old m-machine.h file */

#if SIZEOF_LONG_LONG > 4
#define HAVE_LONG_LONG 1
#endif

#if defined(HAVE_LONG_LONG) && !defined(LONGLONG_MIN)
#define LONGLONG_MIN	((long long) 0x8000000000000000LL)
#define LONGLONG_MAX	((long long) 0x7FFFFFFFFFFFFFFFLL)
#endif

#if SIZEOF_LONG == 4
#define INT_MIN32	(long) 0x80000000L
#define INT_MAX32	(long) 0x7FFFFFFFL
#define INT_MIN24	((long) 0xff800000L)
#define INT_MAX24	0x007fffffL
#define INT_MIN16	((short int) 0x8000)
#define INT_MAX16	0x7FFF
#define INT_MIN8	((char) 0x80)
#define INT_MAX8	((char) 0x7F)
#else  /* Probably Alpha */
#define INT_MIN32	((long) (int) 0x80000000)
#define INT_MAX32	((long) (int) 0x7FFFFFFF)
#define INT_MIN24	((long) (int) 0xff800000)
#define INT_MAX24	((long) (int) 0x007fffff)
#define INT_MIN16	((short int) 0xffff8000)
#define INT_MAX16	((short int) 0x00007FFF)
#endif

/* From limits.h instead */
#ifndef DBL_MIN
#define DBL_MIN		4.94065645841246544e-324
#define FLT_MIN		((float)1.40129846432481707e-45)
#endif
#ifndef DBL_MAX
#define DBL_MAX		1.79769313486231470e+308
#define FLT_MAX		((float)3.40282346638528860e+38)
#endif

/* Max size that must be added to a so that we know Size to make
   adressable obj. */
typedef long		my_ptrdiff_t;
#define MY_ALIGN(A,L)	(((A) + (L) - 1) & ~((L) - 1))
#define ALIGN_SIZE(A)	MY_ALIGN((A),sizeof(double))
/* Size to make adressable obj. */
#define ALIGN_PTR(A, t) ((t*) MY_ALIGN((A),sizeof(t)))
			 /* Offset of filed f in structure t */
#define OFFSET(t, f)	((size_t)(char *)&((t *)0)->f)
#define ADD_TO_PTR(ptr,size,type) (type) ((byte*) (ptr)+size)
#define PTR_BYTE_DIFF(A,B) (my_ptrdiff_t) ((byte*) (A) - (byte*) (B))

#define NullS		(char *) 0
/* Nowdays we do not support MessyDos */
#ifndef NEAR
#define NEAR				/* Who needs segments ? */
#define FAR				/* On a good machine */
#define HUGE_PTR
#endif
#ifndef STDCALL
#define STDCALL
#endif

/* Typdefs for easyier portability */

#if defined(VOIDTYPE)
typedef void	*gptr;		/* Generic pointer */
#else
typedef char	*gptr;		/* Generic pointer */
#endif
#ifndef HAVE_INT_8_16_32
typedef char	int8;		/* Signed integer >= 8	bits */
typedef short	int16;		/* Signed integer >= 16 bits */
#endif
#ifndef HAVE_UCHAR
typedef unsigned char	uchar;	/* Short for unsigned char */
#endif
typedef unsigned char	uint8;	/* Short for unsigned integer >= 8  bits */
typedef unsigned short	uint16; /* Short for unsigned integer >= 16 bits */

#if SIZEOF_INT == 4
#ifndef HAVE_INT_8_16_32
typedef int		int32;
#endif
typedef unsigned int	uint32; /* Short for unsigned integer >= 32 bits */
#elif SIZEOF_LONG == 4
#ifndef HAVE_INT_8_16_32
typedef long		int32;
#endif
typedef unsigned long	uint32; /* Short for unsigned integer >= 32 bits */
#else
error "Neither int or long is of 4 bytes width"
#endif

#if !defined(HAVE_ULONG) && !defined(HAVE_LINUXTHREADS) && !defined(__USE_MISC)
typedef unsigned long	ulong;	/* Short for unsigned long */
#endif
#ifndef longlong_defined
#if defined(HAVE_LONG_LONG) && SIZEOF_LONG != 8
typedef unsigned long long ulonglong;	/* ulong or unsigned long long */
typedef long long	longlong;
#else
typedef unsigned long	ulonglong;	/* ulong or unsigned long long */
typedef long		longlong;
#endif
#endif

#ifdef USE_RAID
/* The following is done with a if to not get problems with pre-processors
   with late define evaluation */
#if SIZEOF_OFF_T == 4
#define SYSTEM_SIZEOF_OFF_T 4
#else
#define SYSTEM_SIZEOF_OFF_T 8
#endif
#undef  SIZEOF_OFF_T
#define SIZEOF_OFF_T	    8
#else
#define SYSTEM_SIZEOF_OFF_T SIZEOF_OFF_T
#endif /* USE_RAID */

#if SIZEOF_OFF_T > 4
typedef ulonglong my_off_t;
#else
typedef unsigned long my_off_t;
#endif
#define MY_FILEPOS_ERROR	(~(my_off_t) 0)
#ifndef __WIN__
typedef off_t os_off_t;
#endif

typedef uint8		int7;	/* Most effective integer 0 <= x <= 127 */
typedef short		int15;	/* Most effective integer 0 <= x <= 32767 */
typedef char		*my_string; /* String of characters */
typedef unsigned long	size_s; /* Size of strings (In string-funcs) */
typedef int		myf;	/* Type of MyFlags in my_funcs */
#ifndef byte_defined
typedef char		byte;	/* Smallest addressable unit */
#endif
typedef char		my_bool; /* Small bool */
#if !defined(bool) && !defined(bool_defined) && (!defined(HAVE_BOOL) || !defined(__cplusplus))
typedef char		bool;	/* Ordinary boolean values 0 1 */
#endif
	/* Macros for converting *constants* to the right type */
#define INT8(v)		(int8) (v)
#define INT16(v)	(int16) (v)
#define INT32(v)	(int32) (v)
#define MYF(v)		(myf) (v)

/* Defines to make it possible to prioritize register assignments. No
   longer needed with moder compilers */
#ifndef USING_X
#define reg1 register
#define reg2 register
#define reg3 register
#define reg4 register
#define reg5 register
#define reg6 register
#define reg7 register
#define reg8 register
#define reg9 register
#define reg10 register
#define reg11 register
#define reg12 register
#define reg13 register
#define reg14 register
#define reg15 register
#define reg16 register
#endif

/* Defines for time function */
#define SCALE_SEC	100
#define SCALE_USEC	10000
#define MY_HOW_OFTEN_TO_ALARM	2	/* How often we want info on screen */
#define MY_HOW_OFTEN_TO_WRITE	1000	/* How often we want info on screen */

/*
** Define-funktions for reading and storing in machine independent format
**  (low byte first)
*/

/* Optimized store functions for Intel x86 */
#ifdef __i386__
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
#endif /* __i386__ */ 

#ifndef sint2korr
#define sint2korr(A)	(int16) (((int16) ((uchar) (A)[0])) +\
				 ((int16) ((int16) (A)[1]) << 8))
#define sint3korr(A)	((int32) ((((uchar) (A)[2]) & 128) ? \
				  (((uint32) 255L << 24) | \
				   (((uint32) (uchar) (A)[2]) << 16) |\
				   (((uint32) (uchar) (A)[1]) << 8) | \
				   ((uint32) (uchar) (A)[0])) : \
				  (((uint32) (uchar) (A)[2]) << 16) |\
				  (((uint32) (uchar) (A)[1]) << 8) | \
				  ((uint32) (uchar) (A)[0])))
#define sint4korr(A)	(int32) (((int32) ((uchar) (A)[0])) +\
				(((int32) ((uchar) (A)[1]) << 8)) +\
				(((int32) ((uchar) (A)[2]) << 16)) +\
				(((int32) ((int16) (A)[3]) << 24)))
#define sint8korr(A)	(longlong) uint8korr(A)
#define uint2korr(A)	(uint16) (((uint16) ((uchar) (A)[0])) +\
				  ((uint16) ((uchar) (A)[1]) << 8))
#define uint3korr(A)	(uint32) (((uint32) ((uchar) (A)[0])) +\
				  (((uint32) ((uchar) (A)[1])) << 8) +\
				  (((uint32) ((uchar) (A)[2])) << 16))
#define uint4korr(A)	(uint32) (((uint32) ((uchar) (A)[0])) +\
				  (((uint32) ((uchar) (A)[1])) << 8) +\
				  (((uint32) ((uchar) (A)[2])) << 16) +\
				  (((uint32) ((uchar) (A)[3])) << 24))
#define uint5korr(A)	((ulonglong)(((uint32) ((uchar) (A)[0])) +\
				    (((uint32) ((uchar) (A)[1])) << 8) +\
				    (((uint32) ((uchar) (A)[2])) << 16) +\
				    (((uint32) ((uchar) (A)[3])) << 24)) +\
			 	    (((ulonglong) ((uchar) (A)[4])) << 32))
#define uint8korr(A)	((ulonglong)(((uint32) ((uchar) (A)[0])) +\
				    (((uint32) ((uchar) (A)[1])) << 8) +\
				    (((uint32) ((uchar) (A)[2])) << 16) +\
				    (((uint32) ((uchar) (A)[3])) << 24)) +\
			(((ulonglong) (((uint32) ((uchar) (A)[4])) +\
				    (((uint32) ((uchar) (A)[5])) << 8) +\
				    (((uint32) ((uchar) (A)[6])) << 16) +\
				    (((uint32) ((uchar) (A)[7])) << 24))) <<\
			 	    32))
#define int2store(T,A)		{ uint def_temp= (uint) (A) ;\
				  *((uchar*) (T))=  (uchar)(def_temp); \
				  *((uchar*) (T+1))=(uchar)((def_temp >> 8)); }
#define int3store(T,A)		{ /*lint -save -e734 */\
				  *((T))=(char) ((A));\
				  *((T)+1)=(char) (((A) >> 8));\
				  *((T)+2)=(char) (((A) >> 16)); \
				  /*lint -restore */}
#define int4store(T,A)		{ *(T)=(char) ((A));\
				  *((T)+1)=(char) (((A) >> 8));\
				  *((T)+2)=(char) (((A) >> 16));\
				  *((T)+3)=(char) (((A) >> 24)); }
#define int5store(T,A)		{ *(T)=((A));\
				  *((T)+1)=(((A) >> 8));\
				  *((T)+2)=(((A) >> 16));\
				  *((T)+3)=(((A) >> 24)); \
				  *((T)+4)=(((A) >> 32)); }
#define int8store(T,A)		{ uint def_temp= (uint) (A), def_temp2= (uint) ((A) >> 32); \
				  int4store((T),def_temp); \
				  int4store((T+4),def_temp2); \
				}
#ifdef WORDS_BIGENDIAN
#define float4store(T,A)    { *(T)= ((byte *) &A)[3];\
                              *((T)+1)=(char) ((byte *) &A)[2];\
                              *((T)+2)=(char) ((byte *) &A)[1];\
                              *((T)+3)=(char) ((byte *) &A)[0]; }

#define float4get(V,M)      { float def_temp;\
                              ((byte*) &def_temp)[0]=(M)[3];\
                              ((byte*) &def_temp)[1]=(M)[2];\
                              ((byte*) &def_temp)[2]=(M)[1];\
                              ((byte*) &def_temp)[3]=(M)[0];\
                              (V)=def_temp; }
#define float8store(T,V)    { *(T)= ((byte *) &V)[7];\
                              *((T)+1)=(char) ((byte *) &V)[6];\
                              *((T)+2)=(char) ((byte *) &V)[5];\
                              *((T)+3)=(char) ((byte *) &V)[4];\
                              *((T)+4)=(char) ((byte *) &V)[3];\
                              *((T)+5)=(char) ((byte *) &V)[2];\
                              *((T)+6)=(char) ((byte *) &V)[1];\
                              *((T)+7)=(char) ((byte *) &V)[0]; }

#define float8get(V,M)	    { double def_temp;\
                              ((byte*) &def_temp)[0]=(M)[7];\
                              ((byte*) &def_temp)[1]=(M)[6];\
                              ((byte*) &def_temp)[2]=(M)[5];\
                              ((byte*) &def_temp)[3]=(M)[4];\
                              ((byte*) &def_temp)[4]=(M)[3];\
                              ((byte*) &def_temp)[5]=(M)[2];\
                              ((byte*) &def_temp)[6]=(M)[1];\
                              ((byte*) &def_temp)[7]=(M)[0];\
			      (V) = def_temp; }
#else
#define float4get(V,M)   memcpy_fixed((byte*) &V,(byte*) (M),sizeof(float))
#define float4store(V,M) memcpy_fixed((byte*) V,(byte*) (&M),sizeof(float))
#define float8get(V,M)   doubleget((V),(M))
#define float8store(V,M) doublestore((V),(M))
#endif /* WORDS_BIGENDIAN */

#endif /* sint2korr */

/* Define-funktions for reading and storing in machine format from/to
   short/long to/from some place in memory V should be a (not
   register) variable, M is a pointer to byte */

#ifdef WORDS_BIGENDIAN

#define ushortget(V,M)	{ V = (uint16) (((uint16) ((uchar) (M)[1]))+\
					((uint16) ((uint16) (M)[0]) << 8)); }
#define shortget(V,M)	{ V = (short) (((short) ((uchar) (M)[1]))+\
				       ((short) ((short) (M)[0]) << 8)); }
#define longget(V,M)	{ int32 def_temp;\
			  ((byte*) &def_temp)[0]=(M)[0];\
			  ((byte*) &def_temp)[1]=(M)[1];\
			  ((byte*) &def_temp)[2]=(M)[2];\
			  ((byte*) &def_temp)[3]=(M)[3];\
			    (V)=def_temp; }
#define ulongget(V,M)	{ uint32 def_temp;\
			  ((byte*) &def_temp)[0]=(M)[0];\
			  ((byte*) &def_temp)[1]=(M)[1];\
			  ((byte*) &def_temp)[2]=(M)[2];\
			  ((byte*) &def_temp)[3]=(M)[3];\
			    (V)=def_temp; }
#define shortstore(T,A) { uint def_temp=(uint) (A) ;\
			  *(T+1)=(char)(def_temp); \
			  *(T+0)=(char)(def_temp >> 8); }
#define longstore(T,A)	{ *((T)+3)=((A));\
			  *((T)+2)=(((A) >> 8));\
			  *((T)+1)=(((A) >> 16));\
			  *((T)+0)=(((A) >> 24)); }

#define doubleget(V,M)	 memcpy((byte*) &V,(byte*) (M),sizeof(double))
#define doublestore(T,V) memcpy((byte*) (T),(byte*) &V,sizeof(double))
#define longlongget(V,M) memcpy((byte*) &V,(byte*) (M),sizeof(ulonglong))
#define longlongstore(T,V) memcpy((byte*) (T),(byte*) &V,sizeof(ulonglong))

#else

#define ushortget(V,M)	{ V = uint2korr(M); }
#define shortget(V,M)	{ V = sint2korr(M); }
#define longget(V,M)	{ V = sint4korr(M); }
#define ulongget(V,M)   { V = uint4korr(M); }
#define shortstore(T,V) int2store(T,V)
#define longstore(T,V)	int4store(T,V)
#ifndef doubleget
#define doubleget(V,M)	 memcpy_fixed((byte*) &V,(byte*) (M),sizeof(double))
#define doublestore(T,V) memcpy_fixed((byte*) (T),(byte*) &V,sizeof(double))
#endif
#define longlongget(V,M) memcpy_fixed((byte*) &V,(byte*) (M),sizeof(ulonglong))
#define longlongstore(T,V) memcpy_fixed((byte*) (T),(byte*) &V,sizeof(ulonglong))

#endif /* WORDS_BIGENDIAN */

/* sprintf does not always return the number of bytes :- */
#ifdef SPRINTF_RETURNS_INT
#define my_sprintf(buff,args) sprintf args
#else
#ifdef SPRINTF_RETURNS_PTR
#define my_sprintf(buff,args) ((int)(sprintf args - buff))
#else
#define my_sprintf(buff,args) sprintf args,strlen(buff)
#endif
#endif


#endif /* _global_h */
