dnl $Id$
dnl config.m4 for extension libming
dnl don't forget to call PHP_EXTENSION(ming)
dnl This file is a modified version of config.m4
dnl in php4/ext/mcrypt

PHP_ARG_WITH(ming, for ming support,
[  --with-ming[=DIR]        Include ming support])

if test "$withval" != "no"; then
  for i in /usr/lib /usr/local /usr $withval; do
    if test -r $i/libming.so; then
      MING_DIR=$i
    fi
  done

  if test -z "$MING_DIR"; then
    AC_MSG_ERROR(Please reinstall libming.so - I cannot find libming.so)
  fi

  AC_ADD_INCLUDE($MING_DIR)

  PHP_SUBST(MING_SHARED_LIBADD)
  AC_ADD_LIBRARY_WITH_PATH(ming, $MING_DIR, MING_SHARED_LIBADD)
  AC_DEFINE(HAVE_MING,1,[ ])

  PHP_EXTENSION(ming, $ext_shared)
fi
