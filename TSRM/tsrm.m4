AC_DEFUN(TSRM_CHECK_GCC_ARG,[
  AC_MSG_CHECKING(whether $CC supports $1)
  > conftest.c
  cmd='$CC $1 -c conftest.c'
  if eval $cmd 2>&1 | egrep -e $1 >/dev/null ; then
    $2=no
  else
    $2=yes
  fi
  AC_MSG_RESULT($$2)
  rm -f conftest.*
])

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

old_LDFLAGS="$LDFLAGS"

if test -n "$GCC"; then
  dnl FreeBSD/Linux 
  TSRM_CHECK_GCC_ARG(-pthread, gcc_pthread)

  if test "$gcc_pthread" = "yes"; then
    LDFLAGS="$LDFLAGS -pthread"
  else
    dnl gcc on Solaris
    TSRM_CHECK_GCC_ARG(-pthreads, gcc_pthreads)
    if test "$gcc_pthreads" = "yes"; then
      LDFLAGS="$LDFLAGS -pthreads"
    fi
  fi

  AC_CHECK_FUNCS(pthread_kill)
fi

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
