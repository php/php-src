dnl
dnl $Id$
dnl 

RESULT=no
AC_MSG_CHECKING(for PHTTPD support)
AC_ARG_WITH(phttpd,
[  --with-phttpd=DIR       Build PHP as phttpd module],[
  PHP_PHTTPD=$withval
], [
  PHP_PHTTPD=no
])

if test "$PHP_PHTTPD" != "no"; then
  if test ! -d $PHP_PHTTPD ; then
    AC_MSG_ERROR(You did not specify a directory)
  fi
  PHP_BUILD_THREAD_SAFE
  PHP_ADD_INCLUDE($PHP_PHTTPD/include)
  AC_DEFINE(HAVE_PHTTPD,1,[Whether you have phttpd])
  PHP_SELECT_SAPI(phttpd, shared, phttpd.c)
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$PHP_PHTTPD/modules/"
  RESULT=yes
fi
AC_MSG_RESULT($RESULT)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
