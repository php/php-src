
AC_DEFUN(TSRM_BASIC_CHECKS,[

AC_REQUIRE([AC_PROG_CC])dnl
dnl AC_REQUIRE([AM_PROG_CC_STDC])dnl
AC_REQUIRE([AC_PROG_CC_C_O])dnl
AC_REQUIRE([AC_PROG_RANLIB])dnl

AC_CHECK_HEADERS(stdarg.h)

])

AC_DEFUN(TSRM_PTHREAD,[

AC_CHECK_LIB(c_r, pthread_kill)
AC_CHECK_LIB(pthread, pthread_kill)

AC_CHECK_FUNCS(pthread_kill)

if test "$ac_cv_func_pthread_kill" != "yes"; then
  AC_MSG_ERROR(You need pthreads to build TSRM.)
fi

AC_DEFINE(PTHREADS, [], Whether to use Pthreads)
])
