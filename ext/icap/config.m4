dnl $Id$

PHP_ARG_WITH(icap,for ICAP support,
[  --with-icap[=DIR]       Include ICAP support.])

if test "$PHP_ICAP" != "no"; then
  ICAP_DIR=$PHP_ICAP
  test "$ICAP_DIR" = "yes" && ICAP_DIR=/usr/local
    
  AC_ADD_INCLUDE($ICAP_DIR)
  if test "$ext_shared" = "yes"; then
    ICAP_SHARED_LIBADD="-R$ICAP_DIR/lib -L$ICAP_DIR/lib -licap"
    PHP_SUBST(ICAP_SHARED_LIBADD)
  else
    AC_ADD_LIBRARY_WITH_PATH(icap, $ICAP_DIR)
  fi
  AC_DEFINE(HAVE_ICAP,1,[ ])
  PHP_EXTENSION(icap, $ext_shared)
fi
