dnl $Id$
dnl config.m4 for extension libswf
dnl don't forget to call PHP_EXTENSION(swf)
dnl This file is a modified version of config.m4
dnl in php4/ext/mcrypt

PHP_ARG_WITH(swf, for libswf support,
[  --with-swf[=DIR]        Include swf support])

if test "$PHP_SWF" != "no"; then
  for i in /usr/local /usr $PHP_SWF; do
    if test -r $i/libswf.a; then
      SWF_DIR=$i
    fi
  done

  if test -z "$SWF_DIR"; then
    AC_MSG_ERROR(Please reinstall libswf.a - I cannot find libswf.a)
  fi

  AC_ADD_INCLUDE($SWF_DIR)

  if test "$ext_shared" = "yes"; then
    SWF_SHARED_LIBADD="-R$SWF_DIR -L$SWF_DIR -lswf"
    PHP_SUBST(SWF_SHARED_LIBADD)
  else
    AC_ADD_LIBRARY_WITH_PATH(swf, $SWF_DIR)
  fi
  AC_DEFINE(HAVE_SWF,1,[ ])

  PHP_EXTENSION(swf, $ext_shared)
fi
