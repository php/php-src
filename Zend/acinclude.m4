dnl $Id$
dnl
dnl This file contains local autoconf functions.

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
    AC_DEFINE(ZEND_BROKEN_SPRINTF, 1, [Whether sprintf is broken])
  else
    AC_DEFINE(ZEND_BROKEN_SPRINTF, 0, [Whether sprintf is broken])
  fi
])

AC_DEFUN(AM_SET_LIBTOOL_VARIABLE,[
  LIBTOOL='$(SHELL) $(top_builddir)/libtool $1'
])

