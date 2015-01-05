dnl
dnl $Id$
dnl 

PHP_ARG_WITH(phttpd, for PHTTPD support,
[  --with-phttpd=DIR       Build PHP as phttpd module], no, no)

if test "$PHP_PHTTPD" != "no"; then
  if test ! -d $PHP_PHTTPD ; then
    AC_MSG_ERROR([You did not specify a directory])
  fi
  PHP_BUILD_THREAD_SAFE
  PHP_ADD_INCLUDE($PHP_PHTTPD/include)
  AC_DEFINE(HAVE_PHTTPD, 1, [Whether you have phttpd])
  PHP_SELECT_SAPI(phttpd, shared, phttpd.c)
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$PHP_PHTTPD/modules/"
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
