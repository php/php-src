
AC_DEFUN(TSRM_BASIC_CHECKS,[

AC_REQUIRE([AC_PROG_CC])dnl
dnl AC_REQUIRE([AM_PROG_CC_STDC])dnl
AC_REQUIRE([AC_PROG_CC_C_O])dnl
AC_REQUIRE([AC_PROG_RANLIB])dnl

AC_CHECK_HEADERS(stdarg.h)

])


AC_DEFUN(TSRM_CHECK_PTH,[

AC_MSG_CHECKING(for GNU Pth)
PTH_PREFIX="`$1 --prefix`"
if test -z "$PTH_PREFIX"; then
  AC_MSG_RESULT(Please check your Pth installation)
fi

CPPFLAGS="$CPPFLAGS `$1 --cflags`"
LDFLAGS="$LDFLAGS `$1 --ldflags`"
LIBS="$LIBS `$1 --libs`"

AC_DEFINE(GNUPTH, 1, [Whether you use GNU Pth])
AC_MSG_RESULT(yes - installed in $PTH_PREFIX)

])


AC_DEFUN(TSRM_CHECK_PTHREADS,[

dnl Check for FreeBSD/Linux -pthread option

old_LDFLAGS="$LDFLAGS"
LDFLAGS="$LDFLAGS -pthread"
AC_CHECK_FUNCS(pthread_kill)

if test "$ac_cv_func_pthread_kill" != "yes"; then
  LDFLAGS="$old_LDFLAGS"
dnl Fall back to the standard -lpthread
  AC_CHECK_LIB(pthread, pthread_kill)
  unset ac_cv_func_pthread_kill
  AC_CHECK_FUNCS(pthread_kill)
  if test "$ac_cv_func_pthread_kill" != "yes"; then
    AC_MSG_ERROR(You need Pthreads to build TSRM on UNIX.)
  fi
fi
		
AC_DEFINE(PTHREADS, [], Whether to use Pthreads)

AC_MSG_CHECKING(for POSIX threads)
AC_MSG_RESULT(yes)
])


AC_DEFUN(TSRM_OTHER_CHECKS,[

dnl For the thread implementations, we always use --with-*
dnl to maintain consistency

AC_ARG_WITH(tsrm-pth,
[  --with-tsrm-pth[=pth-config]    Use GNU Pth.],[
  TSRM_PTH=$withval
],[
  TSRM_PTH=no
])

AC_ARG_WITH(tsrm-pthreads,
[  --with-tsrm-pthreads    Use POSIX threads (default)],[
  TSRM_PTHREADS=$withval
],[
  TSRM_PTHREADS=yes
])

test "$TSRM_PTH" = "yes" && TSRM_PTH=pth-config

if test "$TSRM_PTH" != "no"; then
  TSRM_CHECK_PTH($TSRM_PTH)
elif test "$TSRM_PTHREADS" != "no"; then
  TSRM_CHECK_PTHREADS
fi

])
