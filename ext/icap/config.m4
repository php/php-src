dnl
dnl $Id$
dnl

PHP_ARG_WITH(icap,for ICAP support,
[  --with-icap[=DIR]       Include ICAP support.])

if test "$PHP_ICAP" != "no"; then
  ICAP_DIR=$PHP_ICAP
  test "$ICAP_DIR" = "yes" && ICAP_DIR=/usr/local
    
  PHP_ADD_INCLUDE($ICAP_DIR)
  PHP_SUBST(ICAP_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(icap, $ICAP_DIR, ICAP_SHARED_LIBADD)
  AC_DEFINE(HAVE_ICAP,1,[ ])
  PHP_NEW_EXTENSION(icap, php_icap.c, $ext_shared)
fi
