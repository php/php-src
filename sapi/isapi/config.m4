dnl
dnl $Id$
dnl

PHP_ARG_WITH(isapi, for Zeus ISAPI support,
[  --with-isapi[=DIR]      Build PHP as an ISAPI module for use with Zeus], no, no)

if test "$PHP_ISAPI" != "no"; then
  if test "$PHP_ISAPI" = "yes"; then
    ZEUSPATH=/usr/local/zeus # the default
  else
    ZEUSPATH=$PHP_ISAPI
  fi
  test -f "$ZEUSPATH/web/include/httpext.h" || AC_MSG_ERROR(Unable to find httpext.h in $ZEUSPATH/web/include)
  PHP_BUILD_THREAD_SAFE
  AC_DEFINE(WITH_ZEUS, 1, [ ])
  PHP_ADD_INCLUDE($ZEUSPATH/web/include)
  PHP_SELECT_SAPI(isapi, shared, php5isapi.c)
  INSTALL_IT="\$(SHELL) \$(srcdir)/install-sh -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$ZEUSPATH/web/bin/"
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
