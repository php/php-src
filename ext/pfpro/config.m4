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
    AC_MSG_ERROR(Could not find pfpro.h. Please make sure you have the
                 Verisign Payflow Pro SDK installed. Use
                 ./configure --with-pfpro=<pfpro-dir> if necessary)
  fi

  if test -z "$PFPRO_LIB_DIR"; then
    AC_MSG_ERROR(Could not find libpfpro.so. Please make sure you have the
                 Verisign Payflow Pro SDK installed. Use
                 ./configure --with-pfpro=<pfpro-dir> if necessary)
  fi

  PFPRO_VERSION3=`nm $PFPRO_LIB_DIR/$PFPRO_LIB | awk '{print $3}' | grep '^pfpro' > /dev/null && echo 1 || echo 0`
  PFPRO_VERSION2=`nm $PFPRO_LIB_DIR/$PFPRO_LIB | awk '{print $3}' | grep '^PN' > /dev/null && echo 1 || echo 0`

  if test "$PFPRO_VERSION3" -eq 1 ; then
	PFPRO_VERSION=3
  elif test "$PFPRO_VERSION2" -eq 1 ; then
	PFPRO_VERSION=2
  else
  	AC_MSG_ERROR(The pfpro extension requires version 2 or 3 of the SDK)
  fi
  
  AC_DEFINE_UNQUOTED(PFPRO_VERSION, $PFPRO_VERSION, [Version of SDK])
  dnl AC_MSG_RESULT(found in $PFPRO_LIB_DIR)

  PHP_ADD_INCLUDE($PFPRO_INC_DIR)

  PHP_SUBST(PFPRO_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(pfpro, $PFPRO_LIB_DIR, PFPRO_SHARED_LIBADD)

  AC_DEFINE(HAVE_PFPRO, 1, [ ])

  PHP_EXTENSION(pfpro, $ext_shared)
fi
