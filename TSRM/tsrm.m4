m4_include([TSRM/m4/ax_func_which_gethostbyname_r.m4])

AC_DEFUN([TSRM_BASIC_CHECKS],[

AC_REQUIRE([AC_PROG_CC])dnl
AC_REQUIRE([AC_PROG_RANLIB])dnl

AC_CHECK_FUNCS(sigprocmask)

AX_FUNC_WHICH_GETHOSTBYNAME_R()

])

sinclude(threads.m4)

AC_DEFUN([TSRM_CHECK_PTHREADS],[

PTHREADS_CHECK

if test "$pthreads_working" != "yes"; then
  AC_MSG_ERROR(Your system seems to lack POSIX threads.)
fi

AC_DEFINE(PTHREADS, 1, Whether to use Pthreads)

AC_MSG_CHECKING(for POSIX threads)
AC_MSG_RESULT(yes)
])
