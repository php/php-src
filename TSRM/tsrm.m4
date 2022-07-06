dnl This file contains TSRM specific autoconf macros.

dnl
dnl TSRM_CHECK_PTHREADS
dnl
AC_DEFUN([TSRM_CHECK_PTHREADS],[
PTHREADS_CHECK

if test "$pthreads_working" != "yes"; then
  AC_MSG_ERROR(Your system seems to lack POSIX threads.)
fi

AC_DEFINE(PTHREADS, 1, Whether to use Pthreads)

AC_MSG_CHECKING(for POSIX threads)
AC_MSG_RESULT(yes)
])
