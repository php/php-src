dnl $Id$
dnl
dnl This file contains local autoconf functions.

dnl
dnl Check for broken sprintf()
dnl
AC_DEFUN(AC_ZEND_BROKEN_SPRINTF,[
  AC_MSG_CHECKING([for broken sprintf])
  AC_TRY_RUN([main() { char buf[20]; exit (sprintf(buf,"testing 123")!=11); }],[
    broken=0
    AC_MSG_RESULT(ok)
  ],[
    broken=1
    AC_MSG_RESULT(broken)
  ],[
    broken=0
    AC_MSG_RESULT(cannot check, guessing ok)
  ])
  AC_DEFINE_UNQUOTED(ZEND_BROKEN_SPRINTF, $broken, [Whether sprintf is broken])
])

AC_DEFUN(AM_SET_LIBTOOL_VARIABLE,[
  LIBTOOL='$(SHELL) $(top_builddir)/libtool $1'
])

