/* Defines for Win32 to make it compatible for MySQL */

#include <sys/locking.h>
#include <windows.h>
#include <math.h>			/* Because of rint() */
#include <fcntl.h>
#include <io.h>
#include <malloc.h>

#define	SYSTEM_TYPE	"Win95"
#define MACHINE_TYPE	"i586"		/* Define to machine type name */
#ifndef __WIN32__
#define __WIN32__                       /* To make it easier in VC++ */
#endif

/* File and lock constants */
#define O_SHARE         0x1000 		/* Open file in sharing mode */
#ifdef __BORLANDC__
#define	F_RDLCK		LK_NBLCK	/* read lock */
#define	F_WRLCK		LK_NBRLCK	/* write lock */
#define	F_UNLCK		LK_UNLCK	/* remove lock(s) */
#else
#define	F_RDLCK		_LK_NBLCK	/* read lock */
#define	F_WRLCK		_LK_NBRLCK	/* write lock */
#define	F_UNLCK		_LK_UNLCK	/* remove lock(s) */
#endif

#define F_EXCLUSIVE     1		/* We have only exclusive locking */
#define F_TO_EOF        (INT_MAX32/2)   /* size for lock of all file */
#define F_OK		0		/* parameter to access() */

#define S_IROTH		S_IREAD		/* for my_lib */

#ifdef __BORLANDC__
#define FILE_BINARY	O_BINARY	/* my_fopen in binary mode */
#define O_TEMPORARY	0
#define O_SHORT_LIVED	0
#define SH_DENYNO	_SH_DENYNO
#else
#define O_BINARY        _O_BINARY       /* compability with MSDOS */
#define FILE_BINARY     _O_BINARY       /* my_fopen in binary mode */
#define O_TEMPORARY     _O_TEMPORARY
#define O_SHORT_LIVED   _O_SHORT_LIVED
#define SH_DENYNO       _SH_DENYNO
#endif
#define NO_OPEN_3			/* For my_create() */

#define	SIGQUIT		SIGTERM		/* No SIGQUIT */

#undef _REENTRANT			/* Crashes something for win32 */

#define LONGLONG_MIN	((__int64) 0x8000000000000000)
#define LONGLONG_MAX	((__int64) 0x7FFFFFFFFFFFFFFF)
#define LL(A)		((__int64) A)

/* Type information */

typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef	long off_t ;
typedef unsigned int size_t;
typedef unsigned __int64 ulonglong;	/* Microsofts 64 bit types */
typedef __int64	longlong;
typedef int sigset_t;
#define longlong_defined

#define Socket SOCKET
#define bool BOOL
#define SIGPIPE	SIGINT
#define RETQSORTTYPE void
#define QSORT_TYPE_IS_VOID
#define RETSIGTYPE void
#define SOCKET_SIZE_TYPE int
#define Socket_defined
#define bool_defined
#define byte_defined
#define HUGE_PTR

#define THREAD
#define VOID_SIGHANDLER
#define SIZEOF_CHAR		1
#define SIZEOF_LONG		4
#define SIZEOF_LONG_LONG	8
#define HAVE_BROKEN_NETINET_INCLUDES

/* Convert some simple functions to Posix */

#define sigset(A,B) signal((A),(B))
#define finite(A) _finite(A)
#define sleep(A)  Sleep((A)*1000)

#ifndef __BORLANDC__
#define access(A,B) _access(A,B)
#endif

#if defined(__cplusplus)

inline double rint(double nr)
{
  double f = floor(nr);
  double c = ceil(nr);
  return (((nr-c) >= (nr-f)) ? f :c);
}

inline double ulonglong2double(longlong nr)
{
  if (nr >= 0)
    return (double) nr;
  return (18446744073709551616.0 + (double) nr);
}
#else
#define inline __inline
#endif


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
#define int2store(T,A)	*((uint16*) (T))= (uint16) (A)
#define int3store(T,A)		{ *(T)=  (uchar) ((A));\
				  *(T+1)=(uchar) (((uint) (A) >> 8));\
				  *(T+2)=(uchar) (((A) >> 16)); }
#define int4store(T,A)	*((long *) (T))= (long) (A)

#define doubleget(V,M)	{ *((long *) &V) = *((long*) M); \
			  *(((long *) &V)+1) = *(((long*) M)+1); }
#define doublestore(T,V) { *((long *) T) = *((long*) &V); \
			   *(((long *) T)+1) = *(((long*) &V)+1); }

#define HAVE_PERROR
#define HAVE_VFPRINT
#define HAVE_CHSIZE		/* System has chsize() function */
#define HAVE_RENAME		/* Have rename() as function */
#define HAVE_BINARY_STREAMS	/* Have "b" flag in streams */
#define HAVE_LONG_JMP		/* Have long jump function */
#define HAVE_LOCKING		/* have locking() call */
#define HAVE_ERRNO_AS_DEFINE	/* errno is a define */
#define HAVE_STDLIB		/* everything is include in this file */
#define HAVE_MEMCPY
#define HAVE_MEMMOVE
#define HAVE_GETCWD
#define HAVE_TELL
#define HAVE_PUTENV
#define HAVE_SELECT
#define HAVE_SETLOCALE
#define HAVE_SOCKET		/* Giangi */
#define HAVE_FLOAT_H
#define HAVE_LIMITS_H
#define HAVE_STDDEF_H

#ifdef _MSC_VER
#define HAVE_LDIV		/* The optimizer breaks in zortech for ldiv */
#define HAVE_ANSI_INCLUDE
#define HAVE_SYS_UTIME_H
#define HAVE_STRTOUL
#endif

/* MYSQL OPTIONS */

#define	DEFAULT_MYSQL_HOME	"c:\\mysql"
#define PACKAGE		 	"mysql"
#define PROTOCOL_VERSION	10
