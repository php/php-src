dnl $Id$
dnl
dnl This file contains local autoconf functions.

AC_DEFUN(ZEND_FP_EXCEPT,[
  AC_CACHE_CHECK(whether fp_except is defined, ac_cv_type_fp_except,[
    AC_TRY_COMPILE([
#include <floatingpoint.h>
],[
fp_except x = (fp_except) 0;
],[
     ac_cv_type_fp_except=yes
],[
     ac_cv_type_fp_except=no
],[
     ac_cv_type_fp_except=no
])])
  if test "$ac_cv_type_fp_except" = "yes"; then
    AC_DEFINE(HAVE_FP_EXCEPT, 1, [whether floatingpoint.h defines fp_except])
  fi
])

dnl
dnl Check for broken sprintf()
dnl
AC_DEFUN(AC_ZEND_BROKEN_SPRINTF,[
  AC_CACHE_CHECK(whether sprintf is broken, ac_cv_broken_sprintf,[
    AC_TRY_RUN([main() {char buf[20];exit(sprintf(buf,"testing 123")!=11); }],[
      ac_cv_broken_sprintf=no
    ],[
      ac_cv_broken_sprintf=yes
    ],[
      ac_cv_broken_sprintf=no
    ])
  ])
  if test "$ac_cv_broken_sprintf" = "yes"; then
    ac_result=1
  else
    ac_result=0
  fi
  AC_DEFINE_UNQUOTED(ZEND_BROKEN_SPRINTF, $ac_result, [Whether sprintf is broken])
])

AC_DEFUN(AM_SET_LIBTOOL_VARIABLE,[
  LIBTOOL='$(SHELL) $(top_builddir)/libtool $1'
])

