dnl
dnl $Id$
dnl

PHP_ARG_WITH(pfpro, for Verisign Payflow Pro support,
[  --with-pfpro[=DIR]      Include Verisign Payflow Pro support.])

if test "$PHP_PFPRO" != "no"; then
  PFPRO_LIB=libpfpro.so
  PFPRO_HDR=pfpro.h

  for i in /usr/local /usr $PHP_PFPRO; do
    if test -r $i/$PFPRO_HDR; then
      PFPRO_INC_DIR=$i
    elif test -r $i/include/$PFPRO_HDR; then
      PFPRO_INC_DIR=$i/include
    elif test -r $i/lib/$PFPRO_HDR; then
      PFPRO_INC_DIR=$i/lib
    elif test -r $i/bin/$PFPRO_HDR; then
      PFPRO_INC_DIR=$i/bin
    fi

    if test -r $i/$PFPRO_LIB; then
      PFPRO_LIB_DIR=$i
    elif test -r $i/lib/$PFPRO_LIB; then
      PFPRO_LIB_DIR=$i/lib
    fi
  done

  if test -z "$PFPRO_INC_DIR"; then
    AC_MSG_ERROR([Could not find pfpro.h. Please make sure you have the
                 Verisign Payflow Pro SDK installed. Use
                 ./configure --with-pfpro=<pfpro-dir> if necessary])
  fi

  if test -z "$PFPRO_LIB_DIR"; then
    AC_MSG_ERROR([Could not find libpfpro.so. Please make sure you have the
                 Verisign Payflow Pro SDK installed. Use
                 ./configure --with-pfpro=<pfpro-dir> if necessary])
  fi

  dnl
  dnl Check version of SDK
  dnl
  PHP_CHECK_LIBRARY(pfpro, pfproInit,
  [
    PFPRO_VERSION=3
  ], [
    PHP_CHECK_LIBRARY(pfpro, PNInit,
    [
      PFPRO_VERSION=2
    ], [
      AC_MSG_ERROR([The pfpro extension requires version 2 or 3 of the SDK])
    ], [
      -L$PFPRO_LIB_DIR
    ])
  ], [
    -L$PFPRO_LIB_DIR
  ])

  AC_DEFINE_UNQUOTED(PFPRO_VERSION, $PFPRO_VERSION, [Version of SDK])
  PHP_ADD_INCLUDE($PFPRO_INC_DIR)
  PHP_ADD_LIBRARY_WITH_PATH(pfpro, $PFPRO_LIB_DIR, PFPRO_SHARED_LIBADD)

  PHP_NEW_EXTENSION(pfpro, pfpro.c, $ext_shared)
  PHP_SUBST(PFPRO_SHARED_LIBADD)
  AC_DEFINE(HAVE_PFPRO, 1, [ ])
fi
