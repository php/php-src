#ifndef RUBY_WIN32_H
#define RUBY_WIN32_H 1

#if defined(__cplusplus)
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/*
 *  Copyright (c) 1993, Intergraph Corporation
 *
 *  You may distribute under the terms of either the GNU General Public
 *  License or the Artistic License, as specified in the perl README file.
 *
 */

//
// Definitions for NT port of Perl
//


//
// Ok now we can include the normal include files.
//

// #include <stdarg.h> conflict with varargs.h?
#if !defined(WSAAPI)
#if defined(__cplusplus) && defined(_MSC_VER)
extern "C++" {			/* template without extern "C++" */
#endif
#if !defined(_WIN64) && !defined(WIN32)
#define WIN32
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#if defined(__cplusplus) && defined(_MSC_VER)
}
#endif
#endif

#define NT 1			/* deprecated */

//
// We're not using Microsoft's "extensions" to C for
// Structured Exception Handling (SEH) so we can nuke these
//
#undef try
#undef except
#undef finally
#undef leave

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <process.h>
#include <time.h>
#if defined(__cplusplus) && defined(_MSC_VER) && _MSC_VER == 1200
extern "C++" {			/* template without extern "C++" */
#endif
#include <math.h>
#if defined(__cplusplus) && defined(_MSC_VER) && _MSC_VER == 1200
}
#endif
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef HAVE_SYS_UTIME_H
# include <sys/utime.h>
#else
# include <utime.h>
#endif
#include <io.h>
#include <malloc.h>
#if defined __MINGW32__ || __BORLANDC__ >= 0x0580
# include <stdint.h>
#else
# if !defined(_INTPTR_T_DEFINED)
#  ifdef _WIN64
typedef __int64 intptr_t;
#  else
typedef int intptr_t;
#  endif
#  define _INTPTR_T_DEFINED
# endif
# if !defined(_UINTPTR_T_DEFINED)
#  ifdef _WIN64
typedef unsigned __int64 uintptr_t;
#  else
typedef unsigned int uintptr_t;
#  endif
#  define _UINTPTR_T_DEFINED
# endif
#endif
#ifndef __MINGW32__
# define mode_t int
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef WIN95
extern DWORD rb_w32_osid(void);
#define rb_w32_iswinnt()  (rb_w32_osid() == VER_PLATFORM_WIN32_NT)
#define rb_w32_iswin95()  (rb_w32_osid() == VER_PLATFORM_WIN32_WINDOWS)
#else
#define rb_w32_iswinnt()  TRUE
#define rb_w32_iswin95()  FALSE
#endif

#define WNOHANG -1

#undef getc
#undef putc
#undef fgetc
#undef fputc
#undef getchar
#undef putchar
#undef fgetchar
#undef fputchar
#undef utime
#undef lseek
#undef stat
#undef fstat
#define getc(_stream)		rb_w32_getc(_stream)
#define getchar()		rb_w32_getc(stdin)
#define putc(_c, _stream)	rb_w32_putc(_c, _stream)
#define putchar(_c)		rb_w32_putc(_c, stdout)
#ifdef RUBY_EXPORT
#define fgetc(_stream)		getc(_stream)
#define fputc(_c, _stream)	putc(_c, _stream)
#define fgetchar()		getchar()
#define fputchar(_c)		putchar(_c)
#define utime(_p, _t)		rb_w32_utime(_p, _t)
#define lseek(_f, _o, _w)	_lseeki64(_f, _o, _w)

#define pipe(p)			rb_w32_pipe(p)
#define open			rb_w32_open
#define close(h)		rb_w32_close(h)
#define fclose(f)		rb_w32_fclose(f)
#define read(f, b, s)		rb_w32_read(f, b, s)
#define write(f, b, s)		rb_w32_write(f, b, s)
#define getpid()		rb_w32_getpid()
#define getppid()		rb_w32_getppid()
#define sleep(x)		rb_w32_Sleep((x)*1000)
#define Sleep(msec)		(void)rb_w32_Sleep(msec)
#ifdef __BORLANDC__
#define creat(p, m)		_creat(p, m)
#define eof()			_eof()
#define filelength(h)		_filelength(h)
#define mktemp(t)		_mktemp(t)
#define tell(h)			_tell(h)
#define _open			_sopen
#define sopen			_sopen
#define _fstati64(fd,st)	rb_w32_fstati64(fd,st)
#undef fopen
#define fopen(p, m)		rb_w32_fopen(p, m)
#undef fdopen
#define fdopen(h, m)		rb_w32_fdopen(h, m)
#undef fsopen
#define fsopen(p, m, sh)	rb_w32_fsopen(p, m, sh)
#endif /* __BORLANDC__ */

#undef execv
#define execv(path,argv)	rb_w32_aspawn(P_OVERLAY,path,argv)
#if !defined(__BORLANDC__)
#undef isatty
#define isatty(h)		rb_w32_isatty(h)
#endif /* __BORLANDC__ */

#undef mkdir
#define mkdir(p, m)		rb_w32_mkdir(p, m)
#undef rmdir
#define rmdir(p)		rb_w32_rmdir(p)
#undef unlink
#define unlink(p)		rb_w32_unlink(p)
#endif /* RUBY_EXPORT */

#if SIZEOF_OFF_T == 8
#define off_t __int64
#define stat stati64
#define fstat(fd,st)		_fstati64(fd,st)
#if defined(__BORLANDC__)
#define stati64(path, st) rb_w32_stati64(path, st)
#elif !defined(_MSC_VER) || RT_VER < 80
#define stati64 _stati64
#ifndef _stati64
#define _stati64(path, st) rb_w32_stati64(path, st)
#endif
#else
#define stati64 _stat64
#define _stat64(path, st) rb_w32_stati64(path, st)
#define _fstati64 _fstat64
#endif
#else
#define stat(path,st)		rb_w32_stat(path,st)
#define fstat(fd,st)		rb_w32_fstat(fd,st)
extern int rb_w32_stat(const char *, struct stat *);
extern int rb_w32_fstat(int, struct stat *);
#endif
#define access(path,mode)	rb_w32_access(path,mode)

#define strcasecmp		_stricmp
#define strncasecmp		_strnicmp
#define fsync			_commit

struct timezone;

#ifdef __MINGW32__
#undef isascii
#define isascii __isascii
#endif

struct iovec {
    void *iov_base;
    size_t iov_len;
};
struct msghdr {
    void *msg_name;
    int msg_namelen;
    struct iovec *msg_iov;
    int msg_iovlen;
    void *msg_control;
    int msg_controllen;
    int msg_flags;
};

#define NtInitialize ruby_sysinit
extern int    rb_w32_cmdvector(const char *, char ***);
extern rb_pid_t  rb_w32_pipe_exec(const char *, const char *, int, int *, int *);
extern int    flock(int fd, int oper);
extern int    rb_w32_io_cancelable_p(int);
extern int    rb_w32_is_socket(int);
extern int    WSAAPI rb_w32_accept(int, struct sockaddr *, int *);
extern int    WSAAPI rb_w32_bind(int, const struct sockaddr *, int);
extern int    WSAAPI rb_w32_connect(int, const struct sockaddr *, int);
extern void   rb_w32_fdset(int, fd_set*);
extern void   rb_w32_fdclr(int, fd_set*);
extern int    rb_w32_fdisset(int, fd_set*);
extern int    WSAAPI rb_w32_select(int, fd_set *, fd_set *, fd_set *, struct timeval *);
extern int    WSAAPI rb_w32_getpeername(int, struct sockaddr *, int *);
extern int    WSAAPI rb_w32_getsockname(int, struct sockaddr *, int *);
extern int    WSAAPI rb_w32_getsockopt(int, int, int, char *, int *);
extern int    WSAAPI rb_w32_ioctlsocket(int, long, u_long *);
extern int    WSAAPI rb_w32_listen(int, int);
extern int    WSAAPI rb_w32_recv(int, char *, int, int);
extern int    WSAAPI rb_w32_recvfrom(int, char *, int, int, struct sockaddr *, int *);
extern int    WSAAPI rb_w32_send(int, const char *, int, int);
extern int    WSAAPI rb_w32_sendto(int, const char *, int, int, const struct sockaddr *, int);
extern int    recvmsg(int, struct msghdr *, int);
extern int    sendmsg(int, const struct msghdr *, int);
extern int    WSAAPI rb_w32_setsockopt(int, int, int, const char *, int);
extern int    WSAAPI rb_w32_shutdown(int, int);
extern int    WSAAPI rb_w32_socket(int, int, int);
extern SOCKET rb_w32_get_osfhandle(int);
extern struct hostent *WSAAPI rb_w32_gethostbyaddr(const char *, int, int);
extern struct hostent *WSAAPI rb_w32_gethostbyname(const char *);
extern int    WSAAPI rb_w32_gethostname(char *, int);
extern struct protoent *WSAAPI rb_w32_getprotobyname(const char *);
extern struct protoent *WSAAPI rb_w32_getprotobynumber(int);
extern struct servent  *WSAAPI rb_w32_getservbyname(const char *, const char *);
extern struct servent  *WSAAPI rb_w32_getservbyport(int, const char *);
extern int    rb_w32_socketpair(int, int, int, int *);
extern char * rb_w32_getcwd(char *, int);
extern char * rb_w32_ugetenv(const char *);
extern char * rb_w32_getenv(const char *);
extern int    rb_w32_rename(const char *, const char *);
extern int    rb_w32_urename(const char *, const char *);
extern char **rb_w32_get_environ(void);
extern void   rb_w32_free_environ(char **);
extern int    rb_w32_map_errno(DWORD);
extern char * WSAAPI rb_w32_inet_ntop(int,const void *,char *,size_t);
extern DWORD  rb_w32_osver(void);

extern int chown(const char *, int, int);
extern int rb_w32_uchown(const char *, int, int);
extern int link(const char *, const char *);
extern int rb_w32_ulink(const char *, const char *);
extern int gettimeofday(struct timeval *, struct timezone *);
extern rb_pid_t waitpid (rb_pid_t, int *, int);
extern rb_pid_t rb_w32_spawn(int, const char *, const char*);
extern rb_pid_t rb_w32_aspawn(int, const char *, char *const *);
extern int kill(int, int);
extern int fcntl(int, int, ...);
extern rb_pid_t rb_w32_getpid(void);
extern rb_pid_t rb_w32_getppid(void);
#if !defined(__BORLANDC__)
extern int rb_w32_isatty(int);
#endif
extern int rb_w32_uchdir(const char *);
extern int rb_w32_mkdir(const char *, int);
extern int rb_w32_umkdir(const char *, int);
extern int rb_w32_rmdir(const char *);
extern int rb_w32_urmdir(const char *);
extern int rb_w32_unlink(const char *);
extern int rb_w32_uunlink(const char *);
extern int rb_w32_uchmod(const char *, int);
extern int rb_w32_stati64(const char *, struct stati64 *);
extern int rb_w32_ustati64(const char *, struct stati64 *);
extern int rb_w32_access(const char *, int);
extern int rb_w32_uaccess(const char *, int);
extern char rb_w32_fd_is_text(int);

#ifdef __BORLANDC__
extern int rb_w32_fstati64(int, struct stati64 *);
extern off_t _lseeki64(int, off_t, int);
extern FILE *rb_w32_fopen(const char *, const char *);
extern FILE *rb_w32_fdopen(int, const char *);
extern FILE *rb_w32_fsopen(const char *, const char *, int);
#endif

#include <float.h>
#if !defined __MINGW32__ || defined __NO_ISOCEXT
#ifndef isnan
#define isnan(x) _isnan(x)
#endif
static inline int
finite(double x)
{
    return _finite(x);
}
#ifndef copysign
#define copysign(a, b) _copysign(a, b)
#endif
static inline double
scalb(double a, long b)
{
    return _scalb(a, b);
}
#endif

#if !defined S_IFIFO && defined _S_IFIFO
#define S_IFIFO _S_IFIFO
#endif

#if 0 && defined __BORLANDC__
#undef S_ISDIR
#undef S_ISFIFO
#undef S_ISBLK
#undef S_ISCHR
#undef S_ISREG
#define S_ISDIR(m)  (((unsigned short)(m) & S_IFMT) == S_IFDIR)
#define S_ISFIFO(m) (((unsigned short)(m) & S_IFMT) == S_IFIFO)
#define S_ISBLK(m)  (((unsigned short)(m) & S_IFMT) == S_IFBLK)
#define S_ISCHR(m)  (((unsigned short)(m) & S_IFMT) == S_IFCHR)
#define S_ISREG(m)  (((unsigned short)(m) & S_IFMT) == S_IFREG)
#endif

#if !defined S_IRUSR && !defined __MINGW32__
#define S_IRUSR 0400
#endif
#ifndef S_IRGRP
#define S_IRGRP 0040
#endif
#ifndef S_IROTH
#define S_IROTH 0004
#endif

#if !defined S_IWUSR && !defined __MINGW32__
#define S_IWUSR 0200
#endif
#ifndef S_IWGRP
#define S_IWGRP 0020
#endif
#ifndef S_IWOTH
#define S_IWOTH 0002
#endif

#if !defined S_IXUSR && !defined __MINGW32__
#define S_IXUSR 0100
#endif
#ifndef S_IXGRP
#define S_IXGRP 0010
#endif
#ifndef S_IXOTH
#define S_IXOTH 0001
#endif

//
// define this so we can do inplace editing
//

#define SUFFIX

extern int 	 rb_w32_ftruncate(int fd, off_t length);
extern int 	 rb_w32_truncate(const char *path, off_t length);
extern off_t	 rb_w32_ftello(FILE *stream);
extern int 	 rb_w32_fseeko(FILE *stream, off_t offset, int whence);

#undef HAVE_FTRUNCATE
#define HAVE_FTRUNCATE 1
#if defined HAVE_FTRUNCATE64
#define ftruncate ftruncate64
#else
#define ftruncate rb_w32_ftruncate
#endif

#undef HAVE_TRUNCATE
#define HAVE_TRUNCATE 1
#if defined HAVE_TRUNCATE64
#define truncate truncate64
#else
#define truncate rb_w32_truncate
#endif

#undef HAVE_FSEEKO
#define HAVE_FSEEKO 1
#if defined HAVE_FSEEKO64
#define fseeko fseeko64
#else
#define fseeko rb_w32_fseeko
#endif

#undef HAVE_FTELLO
#define HAVE_FTELLO 1
#if defined HAVE_FTELLO64
#define ftello ftello64
#else
#define ftello rb_w32_ftello
#endif

//
// stubs
//
extern int       ioctl (int, int, ...);
extern rb_uid_t  getuid (void);
extern rb_uid_t  geteuid (void);
extern rb_gid_t  getgid (void);
extern rb_gid_t  getegid (void);
extern int       setuid (rb_uid_t);
extern int       setgid (rb_gid_t);

extern char *rb_w32_strerror(int);

#ifdef RUBY_EXPORT
#define strerror(e) rb_w32_strerror(e)
#endif

#define PIPE_BUF 1024

#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8


#ifndef SIGINT
#define SIGINT 2
#endif
#ifndef SIGKILL
#define SIGKILL	9
#endif


/* #undef va_start */
/* #undef va_end */

/* winsock error map */
#include <errno.h>

#ifndef EWOULDBLOCK
# define EWOULDBLOCK		WSAEWOULDBLOCK
#endif
#ifndef EINPROGRESS
# define EINPROGRESS		WSAEINPROGRESS
#endif
#ifndef EALREADY
# define EALREADY		WSAEALREADY
#endif
#ifndef ENOTSOCK
# define ENOTSOCK		WSAENOTSOCK
#endif
#ifndef EDESTADDRREQ
# define EDESTADDRREQ		WSAEDESTADDRREQ
#endif
#ifndef EMSGSIZE
# define EMSGSIZE		WSAEMSGSIZE
#endif
#ifndef EPROTOTYPE
# define EPROTOTYPE		WSAEPROTOTYPE
#endif
#ifndef ENOPROTOOPT
# define ENOPROTOOPT		WSAENOPROTOOPT
#endif
#ifndef EPROTONOSUPPORT
# define EPROTONOSUPPORT	WSAEPROTONOSUPPORT
#endif
#ifndef ESOCKTNOSUPPORT
# define ESOCKTNOSUPPORT	WSAESOCKTNOSUPPORT
#endif
#ifndef EOPNOTSUPP
# define EOPNOTSUPP		WSAEOPNOTSUPP
#endif
#ifndef EPFNOSUPPORT
# define EPFNOSUPPORT		WSAEPFNOSUPPORT
#endif
#ifndef EAFNOSUPPORT
# define EAFNOSUPPORT		WSAEAFNOSUPPORT
#endif
#ifndef EADDRINUSE
# define EADDRINUSE		WSAEADDRINUSE
#endif
#ifndef EADDRNOTAVAIL
# define EADDRNOTAVAIL		WSAEADDRNOTAVAIL
#endif
#ifndef ENETDOWN
# define ENETDOWN		WSAENETDOWN
#endif
#ifndef ENETUNREACH
# define ENETUNREACH		WSAENETUNREACH
#endif
#ifndef ENETRESET
# define ENETRESET		WSAENETRESET
#endif
#ifndef ECONNABORTED
# define ECONNABORTED		WSAECONNABORTED
#endif
#ifndef ECONNRESET
# define ECONNRESET		WSAECONNRESET
#endif
#ifndef ENOBUFS
# define ENOBUFS		WSAENOBUFS
#endif
#ifndef EISCONN
# define EISCONN		WSAEISCONN
#endif
#ifndef ENOTCONN
# define ENOTCONN		WSAENOTCONN
#endif
#ifndef ESHUTDOWN
# define ESHUTDOWN		WSAESHUTDOWN
#endif
#ifndef ETOOMANYREFS
# define ETOOMANYREFS		WSAETOOMANYREFS
#endif
#ifndef ETIMEDOUT
# define ETIMEDOUT		WSAETIMEDOUT
#endif
#ifndef ECONNREFUSED
# define ECONNREFUSED		WSAECONNREFUSED
#endif
#ifndef ELOOP
# define ELOOP			WSAELOOP
#endif
/*#define ENAMETOOLONG	WSAENAMETOOLONG*/
#ifndef EHOSTDOWN
# define EHOSTDOWN		WSAEHOSTDOWN
#endif
#ifndef EHOSTUNREACH
# define EHOSTUNREACH		WSAEHOSTUNREACH
#endif
/*#define ENOTEMPTY	WSAENOTEMPTY*/
#ifndef EPROCLIM
# define EPROCLIM		WSAEPROCLIM
#endif
#ifndef EUSERS
# define EUSERS			WSAEUSERS
#endif
#ifndef EDQUOT
# define EDQUOT			WSAEDQUOT
#endif
#ifndef ESTALE
# define ESTALE			WSAESTALE
#endif
#ifndef EREMOTE
# define EREMOTE		WSAEREMOTE
#endif

#define F_DUPFD 0
//#define F_GETFD 1
//#define F_SETFD 2
//#define F_GETFL 3
#define F_SETFL 4
//#define FD_CLOEXEC 1 /* F_GETFD, F_SETFD */
#define O_NONBLOCK 1

#undef FD_SET
#define FD_SET(f, s)		rb_w32_fdset(f, s)

#undef FD_CLR
#define FD_CLR(f, s)		rb_w32_fdclr(f, s)

#undef FD_ISSET
#define FD_ISSET(f, s)		rb_w32_fdisset(f, s)

#ifdef RUBY_EXPORT
#undef inet_ntop
#define inet_ntop(f,a,n,l)      rb_w32_inet_ntop(f,a,n,l)

#undef accept
#define accept(s, a, l)		rb_w32_accept(s, a, l)

#undef bind
#define bind(s, a, l)		rb_w32_bind(s, a, l)

#undef connect
#define connect(s, a, l)	rb_w32_connect(s, a, l)

#undef select
#define select(n, r, w, e, t)	rb_w32_select(n, r, w, e, t)

#undef getpeername
#define getpeername(s, a, l)	rb_w32_getpeername(s, a, l)

#undef getsockname
#define getsockname(s, a, l)	rb_w32_getsockname(s, a, l)

#undef getsockopt
#define getsockopt(s, v, n, o, l) rb_w32_getsockopt(s, v, n, o, l)

#undef ioctlsocket
#define ioctlsocket(s, c, a)	rb_w32_ioctlsocket(s, c, a)

#undef listen
#define listen(s, b)		rb_w32_listen(s, b)

#undef recv
#define recv(s, b, l, f)	rb_w32_recv(s, b, l, f)

#undef recvfrom
#define recvfrom(s, b, l, f, fr, frl) rb_w32_recvfrom(s, b, l, f, fr, frl)

#undef send
#define send(s, b, l, f)	rb_w32_send(s, b, l, f)

#undef sendto
#define sendto(s, b, l, f, t, tl) rb_w32_sendto(s, b, l, f, t, tl)

#undef setsockopt
#define setsockopt(s, v, n, o, l) rb_w32_setsockopt(s, v, n, o, l)

#undef shutdown
#define shutdown(s, h)		rb_w32_shutdown(s, h)

#undef socket
#define socket(s, t, p)		rb_w32_socket(s, t, p)

#undef gethostbyaddr
#define gethostbyaddr(a, l, t)	rb_w32_gethostbyaddr(a, l, t)

#undef gethostbyname
#define gethostbyname(n)	rb_w32_gethostbyname(n)

#undef gethostname
#define gethostname(n, l)	rb_w32_gethostname(n, l)

#undef getprotobyname
#define getprotobyname(n)	rb_w32_getprotobyname(n)

#undef getprotobynumber
#define getprotobynumber(n)	rb_w32_getprotobynumber(n)

#undef getservbyname
#define getservbyname(n, p)	rb_w32_getservbyname(n, p)

#undef getservbyport
#define getservbyport(p, pr)	rb_w32_getservbyport(p, pr)

#undef socketpair
#define socketpair(a, t, p, s)	rb_w32_socketpair(a, t, p, s)

#undef get_osfhandle
#define get_osfhandle(h)	rb_w32_get_osfhandle(h)

#undef getcwd
#define getcwd(b, s)		rb_w32_getcwd(b, s)

#undef getenv
#define getenv(n)		rb_w32_getenv(n)

#undef rename
#define rename(o, n)		rb_w32_rename(o, n)

#undef times
#define times(t)		rb_w32_times(t)
#endif

struct tms {
	long	tms_utime;
	long	tms_stime;
	long	tms_cutime;
	long	tms_cstime;
};

int rb_w32_times(struct tms *);

/* thread stuff */
int  rb_w32_sleep(unsigned long msec);
int  rb_w32_putc(int, FILE*);
int  rb_w32_getc(FILE*);
int  rb_w32_open(const char *, int, ...);
int  rb_w32_uopen(const char *, int, ...);
int  rb_w32_wopen(const WCHAR *, int, ...);
int  rb_w32_close(int);
int  rb_w32_fclose(FILE*);
int  rb_w32_pipe(int[2]);
ssize_t rb_w32_read(int, void *, size_t);
ssize_t rb_w32_write(int, const void *, size_t);
int  rb_w32_utime(const char *, const struct utimbuf *);
int  rb_w32_uutime(const char *, const struct utimbuf *);
long rb_w32_write_console(uintptr_t, int);	/* use uintptr_t instead of VALUE because it's not defined yet here */
int  WINAPI rb_w32_Sleep(unsigned long msec);
int  rb_w32_wait_events_blocking(HANDLE *events, int num, DWORD timeout);
int  rb_w32_time_subtract(struct timeval *rest, const struct timeval *wait);

/*
== ***CAUTION***
Since this function is very dangerous, ((*NEVER*))
* lock any HANDLEs(i.e. Mutex, Semaphore, CriticalSection and so on) or,
* use anything like TRAP_BEG...TRAP_END block structure,
in asynchronous_func_t.
*/
typedef uintptr_t (*asynchronous_func_t)(uintptr_t self, int argc, uintptr_t* argv);
uintptr_t rb_w32_asynchronize(asynchronous_func_t func, uintptr_t self, int argc, uintptr_t* argv, uintptr_t intrval);

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#ifdef __MINGW_ATTRIB_PURE
/* License: Ruby's */
/* get rid of bugs in math.h of mingw */
#define frexp(_X, _Y) __extension__ ({\
    int intpart_frexp_bug = intpart_frexp_bug;\
    double result_frexp_bug = frexp((_X), &intpart_frexp_bug);\
    *(_Y) = intpart_frexp_bug;\
    result_frexp_bug;\
})
/* License: Ruby's */
#define modf(_X, _Y) __extension__ ({\
    double intpart_modf_bug = intpart_modf_bug;\
    double result_modf_bug = modf((_X), &intpart_modf_bug);\
    *(_Y) = intpart_modf_bug;\
    result_modf_bug;\
})
#endif

#if defined(__cplusplus)
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif

#endif /* RUBY_WIN32_H */
