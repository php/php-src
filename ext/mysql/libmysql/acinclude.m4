# Local macros for automake & autoconf

AC_DEFUN(MYSQL_FUNCTION_CHECKS,[

# Standard MySQL list
AC_CHECK_FUNCS(alarm bmove \
 chsize ftruncate rint finite fpsetmask fpresetsticky\
 cuserid fcntl fconvert  \
 getrusage getpwuid getcwd getrlimit getwd index locking longjmp \
 perror pread realpath rename \
 socket strnlen madvise \
 strtoll strtoul strtoull snprintf tempnam thr_setconcurrency \
 gethostbyaddr_r gethostbyname_r getpwnam \
 bfill bzero bcmp strstr strpbrk strerror\
 tell atod memcpy memmove \
 setupterm strcasecmp sighold \
 vidattr setupterm lrand48 localtime_r \
 sigset sigthreadmask pthread_sigmask pthread_setprio pthread_setprio_np \
 pthread_setschedparam pthread_attr_setprio pthread_attr_setschedparam \
 pthread_attr_create pthread_getsequence_np pthread_attr_setstacksize \
 pthread_condattr_create rwlock_init \
 crypt dlopen dlerror fchmod getpass getpassphrase)

# This is special for libmysql
AC_CHECK_FUNCS(strtok_r)

MYSQL_CHECK_GETHOSTNAME_R
])

AC_DEFUN(MYSQL_CHECK_GETHOSTNAME_R,[
# Check definition of gethostbyname_r (glibc2.0.100 is different from Solaris)
ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([style of gethostname_r routines], mysql_cv_gethostname_style,
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
if test "$ac_cv_prog_gxx" = "yes"
then
  CXXFLAGS="$CXXFLAGS -Werror"
fi
AC_TRY_COMPILE(
[#if !defined(SCO) && !defined(__osf__)
#define _REENTRANT
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>],
[int skr;

 skr = gethostbyname_r((const char *) 0,
  (struct hostent*) 0, (char*) 0, 0, (struct hostent **) 0, &skr);],
mysql_cv_gethostname_style=glibc2, mysql_cv_gethostname_style=other))
AC_LANG_RESTORE
CXXFLAGS="$ac_save_CXXFLAGS"
if test "$mysql_cv_gethostname_style" = "glibc2"
then
  AC_DEFINE(HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE, , [ ])
fi

# Check 3rd argument of getthostbyname_r
ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([3 argument to gethostname_r routines], mysql_cv_gethostname_arg,
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
if test "$ac_cv_prog_gxx" = "yes"
then
  CXXFLAGS="$CXXFLAGS -Werror"
fi
AC_TRY_COMPILE(
[#if !defined(SCO) && !defined(__osf__)
#define _REENTRANT
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>],
[int skr;

 skr = gethostbyname_r((const char *) 0, (struct hostent*) 0, (hostent_data*) 0);],
mysql_cv_gethostname_arg=hostent_data, mysql_cv_gethostname_arg=char))
AC_LANG_RESTORE
CXXFLAGS="$ac_save_CXXFLAGS"
if test "$mysql_cv_gethostname_arg" = "hostent_data"
then
  AC_DEFINE(HAVE_GETHOSTBYNAME_R_RETURN_INT, , [ ])
fi
])

AC_DEFUN(MYSQL_CHECK_SIGWAIT_STYLE,[
# Check definition av posix sigwait()
AC_CACHE_CHECK("style of sigwait", mysql_cv_sigwait,
AC_TRY_LINK(
[#ifndef SCO
#define _REENTRANT
#endif
#define _POSIX_PTHREAD_SEMANTICS 
#include <pthread.h>
#include <signal.h>],
[#ifndef _AIX
sigset_t set;
int sig;
sigwait(&set,&sig);
#endif],
mysql_cv_sigwait=POSIX, mysql_cv_sigwait=other))
if test "$mysql_cv_sigwait" = "POSIX"
then
  AC_DEFINE(HAVE_SIGWAIT,,[ ])
fi

if test "$mysql_cv_sigwait" != "POSIX"
then
unset mysql_cv_sigwait
# Check definition av posix sigwait()
AC_CACHE_CHECK("style of sigwait", mysql_cv_sigwait,
AC_TRY_LINK(
[#ifndef SCO
#define _REENTRANT
#endif
#define _POSIX_PTHREAD_SEMANTICS 
#include <pthread.h>
#include <signal.h>],
[sigset_t set;
int sig;
sigwait(&set);],
mysql_cv_sigwait=NONPOSIX, mysql_cv_sigwait=other))
if test "$mysql_cv_sigwait" = "NONPOSIX"
then
  AC_DEFINE(HAVE_NONPOSIX_SIGWAIT,,[ ])
fi
fi
])

AC_DEFUN(MYSQL_CHECK_READDIR_R_ARGS,[
# Check definition of readdir_r
AC_CACHE_CHECK("args to readdir_r", mysql_cv_readdir_r,
AC_TRY_LINK(
[#ifndef SCO
#define _REENTRANT
#endif
#define _POSIX_PTHREAD_SEMANTICS 
#include <pthread.h>
#include <dirent.h>],
[ int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
readdir_r((DIR *) NULL, (struct dirent *) NULL, (struct dirent **) NULL); ],
mysql_cv_readdir_r=POSIX, mysql_cv_readdir_r=other))
if test "$mysql_cv_readdir_r" = "POSIX"
then
  AC_DEFINE(HAVE_READDIR_R,,[ ])
fi
])

AC_DEFUN(MYSQL_CHECK_PTHREAD_MUTEX_INIT,[
  # Check definition of pthread_mutex_init
  AC_CACHE_CHECK("args to pthread_mutex_init", mysql_cv_mutex_init_args,
  AC_TRY_COMPILE(
[#ifndef SCO
#define _REENTRANT
#endif
#define _POSIX_PTHREAD_SEMANTICS 
#include <pthread.h> ],
[ 
  pthread_mutexattr_t attr;
  pthread_mutex_t mp;
  pthread_mutex_init(&mp,&attr); ],
mysql_cv_mutex_init_args=POSIX, mysql_cv_mutex_init_args=other))
  if test "$mysql_cv_mutex_init_args" = "other"
  then
    AC_DEFINE(HAVE_NONPOSIX_PTHREAD_MUTEX_INIT,,[ ])
  fi
])

AC_DEFUN(MYSQL_CHECK_PTHREAD_GETSPECIFIC,[
  # Check definition of pthread_getspecific
  AC_CACHE_CHECK("args to pthread_getspecific", mysql_cv_getspecific_args,
  AC_TRY_COMPILE(
[#ifndef SCO
#define _REENTRANT
#endif
#define _POSIX_PTHREAD_SEMANTICS 
#include <pthread.h> ],
[ void *pthread_getspecific(pthread_key_t key);
pthread_getspecific((pthread_key_t) NULL); ],
mysql_cv_getspecific_args=POSIX, mysql_cv_getspecific_args=other))
  if test "$mysql_cv_getspecific_args" = "other"
  then
    AC_DEFINE(HAVE_NONPOSIX_PTHREAD_GETSPECIFIC,,[ ])
  fi
])

AC_DEFUN(MYSQL_TYPE_ACCEPT,
[ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([base type of last arg to accept], mysql_cv_btype_last_arg_accept,
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
if test "$ac_cv_prog_gxx" = "yes"
then
  CXXFLAGS="$CXXFLAGS -Werror"
fi
mysql_cv_btype_last_arg_accept=none
[AC_TRY_COMPILE([#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
],
[int a = accept(1, (struct sockaddr *) 0, (socklen_t *) 0);],
mysql_cv_btype_last_arg_accept=socklen_t)]
if test $mysql_cv_btype_last_arg_accept = none; then
[AC_TRY_COMPILE([#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
],
[int a = accept(1, (struct sockaddr *) 0, (size_t *) 0);],
mysql_cv_btype_last_arg_accept=size_t)]
fi
if test $mysql_cv_btype_last_arg_accept = none; then
mysql_cv_btype_last_arg_accept=int
fi)
AC_LANG_RESTORE
AC_DEFINE_UNQUOTED(SOCKET_SIZE_TYPE, $mysql_cv_btype_last_arg_accept,[ ])
CXXFLAGS="$ac_save_CXXFLAGS"
])

dnl Find type of qsort
AC_DEFUN(MYSQL_TYPE_QSORT,
[AC_CACHE_CHECK([return type of qsort], mysql_cv_type_qsort,
[AC_TRY_COMPILE([#include <stdlib.h>
#ifdef __cplusplus                                                              extern "C"
#endif
void qsort(void *base, size_t nel, size_t width,
 int (*compar) (const void *, const void *));
],
[int i;], mysql_cv_type_qsort=void, mysql_cv_type_qsort=int)])
AC_DEFINE_UNQUOTED(RETQSORTTYPE, $mysql_cv_type_qsort, [ ])
if test "$mysql_cv_type_qsort" = "void"
then
 AC_DEFINE_UNQUOTED(QSORT_TYPE_IS_VOID, 1, [ ])
fi                                                                              ])


#---START: Used in for client configure
AC_DEFUN(MYSQL_CHECK_ULONG,
[AC_MSG_CHECKING(for type ulong)
AC_CACHE_VAL(ac_cv_ulong,
[AC_TRY_RUN([#include <stdio.h>
#include <sys/types.h>
main()
{
  ulong foo;
  foo++;
  exit(0);
}], ac_cv_ulong=yes, ac_cv_ulong=no, ac_cv_ulong=no)])
AC_MSG_RESULT($ac_cv_ulong)
if test "$ac_cv_ulong" = "yes"
then
  AC_DEFINE(HAVE_ULONG,,[ ])
fi
])

AC_DEFUN(MYSQL_CHECK_UCHAR,
[AC_MSG_CHECKING(for type uchar)
AC_CACHE_VAL(ac_cv_uchar,
[AC_TRY_RUN([#include <stdio.h>
#include <sys/types.h>
main()
{
  uchar foo;
  foo++;
  exit(0);
}], ac_cv_uchar=yes, ac_cv_uchar=no, ac_cv_uchar=no)])
AC_MSG_RESULT($ac_cv_uchar)
if test "$ac_cv_uchar" = "yes"
then
  AC_DEFINE(HAVE_UCHAR,,[ ])
fi
])

AC_DEFUN(MYSQL_CHECK_UINT,
[AC_MSG_CHECKING(for type uint)
AC_CACHE_VAL(ac_cv_uint,
[AC_TRY_RUN([#include <stdio.h>
#include <sys/types.h>
main()
{
  uint foo;
  foo++;
  exit(0);
}], ac_cv_uint=yes, ac_cv_uint=no, ac_cv_uint=no)])
AC_MSG_RESULT($ac_cv_uint)
if test "$ac_cv_uint" = "yes"
then
  AC_DEFINE(HAVE_UINT,,[ ])
fi
])

AC_DEFUN(MYSQL_CHECK_USHORT,
[AC_MSG_CHECKING(for type ushort)
AC_CACHE_VAL(ac_cv_ushort,
[AC_TRY_RUN([#include <stdio.h>
#include <sys/types.h>
main()
{
  ushort foo;
  foo++;
  exit(0);
}], ac_cv_ushort=yes, ac_cv_ushort=no, ac_cv_ushort=no)])
AC_MSG_RESULT($ac_cv_ushort)
if test "$ac_cv_ushort" = "yes"
then
  AC_DEFINE(HAVE_USHORT,,[ ])
fi
])

AC_DEFUN(MYSQL_CHECK_INT_8_16_32,
[AC_MSG_CHECKING([for int8])
AC_CACHE_VAL(ac_cv_int8,
[AC_TRY_RUN([
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

int main()
{
    int8 i;
    return 0;
}
], ac_cv_int8=yes, ac_cv_int8=no, ac_cv_int8=no)])
AC_MSG_RESULT($ac_cv_int8)
if test "$ac_cv_int8" = "yes"
then
  AC_DEFINE(HAVE_INT_8_16_32,,[ ])
fi
])


AC_DEFUN(MYSQL_HEADER_CHECKS,[
AC_HEADER_STDC
AC_CHECK_HEADERS(sgtty.h sys/ioctl.h \
 fcntl.h float.h floatingpoint.h ieeefp.h limits.h \
 memory.h pwd.h select.h \
 stdlib.h stddef.h \
 strings.h string.h synch.h sys/mman.h sys/socket.h \
 sys/timeb.h sys/types.h sys/un.h sys/vadvise.h sys/wait.h term.h \
 unistd.h utime.h sys/utime.h termio.h termios.h sched.h crypt.h alloca.h)
])

AC_DEFUN(MYSQL_TYPE_CHECKS,[

AC_REQUIRE([AC_C_CONST])
AC_REQUIRE([AC_C_INLINE])
AC_CHECK_SIZEOF(char, 1)

AC_CHECK_SIZEOF(int, 4)
AC_CHECK_SIZEOF(long, 4)
AC_CHECK_SIZEOF(long long, 8)
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_TYPE_UID_T

MYSQL_CHECK_ULONG
MYSQL_CHECK_UCHAR
MYSQL_CHECK_UINT
MYSQL_CHECK_USHORT
MYSQL_CHECK_INT_8_16_32

MYSQL_TYPE_ACCEPT
MYSQL_TYPE_QSORT

AC_REQUIRE([AC_TYPE_SIGNAL])
])
