dnl $Id$
dnl config.m4 for extension pfpro

AC_MSG_CHECKING(for Payflow Pro support)

PHP_ARG_WITH(pfpro, for pfpro support,
[  --with-pfpro=[DIR]       Include Payflow Pro support])

if test "$PHP_PFPRO" != "no"; then
  if test -r $PHP_PFPRO/lib/libpfpro.so; then
    PFPRO_DIR=$PHP_PFPRO
  else
    AC_MSG_CHECKING(for libpfpro in default path)
    for i in /usr/local /usr; do
      if test -r $i/lib/libpfpro.so; then
        PFPRO_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  if test -z "$PFPRO_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please install the payflow pro SDK distribution -
   pfpro.h should be in <pfpro-dir>/include and
   libpfpro.so should be in <pfpro-dir>/lib)
   Use ./configure --with-pfpro=<pfpro-dir> if necessary
  fi

  AC_ADD_INCLUDE($PFPRO_DIR/include)

  PHP_SUBST(PFPRO_SHARED_LIBADD)

  AC_ADD_LIBRARY_WITH_PATH(pfpro, $PFPRO_DIR, PFPRO_SHARED_LIBADD)

  AC_DEFINE(HAVE_PFPRO, 1, [ ])

  PHP_EXTENSION(pfpro, $ext_shared)
fi
