dnl $Id$
dnl config.m4 for extension pfpro

PHP_ARG_WITH(pfpro, whether to include Verisign Payflow Pro support,
[  --with-pfpro[=DIR]      Include Verisign Payflow Pro support])

if test "$PHP_PFPRO" != "no"; then

  for i in /usr/local /usr $PHP_PFPRO; do
    if test -r $i/pfpro.h; then
      PFPRO_INC_DIR=$i
    elif test -r $i/include/pfpro.h; then
      PFPRO_INC_DIR=$i/include
    fi

    if test -r $i/libpfpro.so; then
      PFPRO_LIB_DIR=$i
    elif test -r $i/lib/libpfpro.so; then
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

  AC_MSG_RESULT(found in $PFPRO_LIB_DIR)

  PHP_ADD_INCLUDE($PFPRO_INC_DIR)

  PHP_SUBST(PFPRO_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(pfpro, $PFPRO_LIB_DIR, PFPRO_SHARED_LIBADD)

  AC_DEFINE(HAVE_PFPRO, 1, [ ])

  PHP_EXTENSION(pfpro, $ext_shared)
fi
