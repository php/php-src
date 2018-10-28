dnl  Copyright (c) 1999, 2000 Sascha Schumann. All rights reserved.
dnl
dnl  Redistribution and use in source and binary forms, with or without
dnl  modification, are permitted provided that the following conditions
dnl  are met:
dnl
dnl  1. Redistributions of source code must retain the above copyright
dnl     notice, this list of conditions and the following disclaimer.
dnl
dnl  2. Redistributions in binary form must reproduce the above copyright
dnl     notice, this list of conditions and the following disclaimer in
dnl     the documentation and/or other materials provided with the
dnl     distribution.
dnl
dnl  THIS SOFTWARE IS PROVIDED BY SASCHA SCHUMANN ``AS IS'' AND ANY
dnl  EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
dnl  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
dnl  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL SASCHA SCHUMANN OR
dnl  HIS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
dnl  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
dnl  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
dnl  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
dnl  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
dnl  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
dnl  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
dnl  OF THE POSSIBILITY OF SUCH DAMAGE.

dnl
dnl PTHREADS_FLAGS
dnl
dnl Set some magic defines to achieve POSIX threads conformance
dnl
AC_DEFUN([PTHREADS_FLAGS],[
  if test -z "$host_alias" && test -n "$host"; then
    host_alias=$host
  fi
  if test -z "$host_alias"; then
    AC_MSG_ERROR(host_alias is not set. Make sure to run config.guess)
  fi
  case $host_alias in
  *solaris*)
    PTHREAD_FLAGS="-D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT";;
  *freebsd*)
    PTHREAD_FLAGS="-D_REENTRANT -D_THREAD_SAFE";;
  *linux*)
    PTHREAD_FLAGS=-D_REENTRANT;;
  *aix*)
    PTHREAD_FLAGS=-D_THREAD_SAFE;;
  *irix*)
    PTHREAD_FLAGS=-D_POSIX_THREAD_SAFE_FUNCTIONS;;
  *hpux*)
    PTHREAD_FLAGS=-D_REENTRANT;;
  *sco*)
    PTHREAD_FLAGS=-D_REENTRANT;;
dnl Solves sigwait() problem, creates problems with u_long etc.
dnl    PTHREAD_FLAGS="-D_REENTRANT -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=199506 -D_XOPEN_SOURCE_EXTENDED=1";;
  esac

  if test -n "$PTHREAD_FLAGS"; then
    CPPFLAGS="$CPPFLAGS $PTHREAD_FLAGS"
  fi
])dnl
dnl
dnl PTHREADS_CHECK_COMPILE
dnl
dnl Check whether the current setup can use POSIX threads calls
dnl
AC_DEFUN([PTHREADS_CHECK_COMPILE], [
AC_LINK_IFELSE([ AC_LANG_SOURCE([
#include <pthread.h>
#include <stddef.h>

void *thread_routine(void *data) {
    return data;
}

int main() {
    pthread_t thd;
    pthread_mutexattr_t mattr;
    int data = 1;
    pthread_mutexattr_init(&mattr);
    return pthread_create(&thd, NULL, thread_routine, &data);
} ]) ], [
  pthreads_checked=yes
  ], [
  pthreads_checked=no
  ]
) ] )dnl
dnl
dnl PTHREADS_CHECK()
dnl
dnl Try to find a way to enable POSIX threads
dnl
dnl  Magic flags
dnl  -kthread          gcc (FreeBSD)
dnl  -Kthread          UDK cc (UnixWare)
dnl  -mt               WorkShop cc (Solaris)
dnl  -mthreads         gcc (AIX)
dnl  -pthread          gcc (Linux, FreeBSD, NetBSD, OpenBSD)
dnl  -pthreads         gcc (Solaris)
dnl  -qthreaded        AIX cc V5
dnl  -threads          gcc (HP-UX)
dnl
AC_DEFUN([PTHREADS_CHECK],[

save_CFLAGS=$CFLAGS
save_LIBS=$LIBS
PTHREADS_ASSIGN_VARS
PTHREADS_CHECK_COMPILE
LIBS=$save_LIBS
CFLAGS=$save_CFLAGS

AC_CACHE_CHECK(for pthreads_cflags,ac_cv_pthreads_cflags,[
ac_cv_pthreads_cflags=
if test "$pthreads_working" != "yes"; then
  for flag in -kthread -pthread -pthreads -mthreads -Kthread -threads -mt -qthreaded; do
    ac_save=$CFLAGS
    CFLAGS="$CFLAGS $flag"
    PTHREADS_CHECK_COMPILE
    CFLAGS=$ac_save
    if test "$pthreads_checked" = "yes"; then
      ac_cv_pthreads_cflags=$flag
      break
    fi
  done
fi
])

AC_CACHE_CHECK(for pthreads_lib, ac_cv_pthreads_lib,[
ac_cv_pthreads_lib=
if test "$pthreads_working" != "yes"; then
 for lib in pthread pthreads c_r; do
    ac_save=$LIBS
    LIBS="$LIBS -l$lib"
    PTHREADS_CHECK_COMPILE
    LIBS=$ac_save
    if test "$pthreads_checked" = "yes"; then
      ac_cv_pthreads_lib=$lib
      break
    fi
  done
fi
])

if test "x$ac_cv_pthreads_cflags" != "x" -o "x$ac_cv_pthreads_lib" != "x"; then
  pthreads_working="yes"
fi

if test "$pthreads_working" = "yes"; then
  threads_result="POSIX-Threads found"
else
  threads_result="POSIX-Threads not found"
fi
])dnl
dnl
dnl
AC_DEFUN([PTHREADS_ASSIGN_VARS],[
if test -n "$ac_cv_pthreads_lib"; then
  LIBS="$LIBS -l$ac_cv_pthreads_lib"
fi

if test -n "$ac_cv_pthreads_cflags"; then
  CFLAGS="$CFLAGS $ac_cv_pthreads_cflags"
fi
])dnl
