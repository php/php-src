dnl $Id$
dnl config.m4 for extension libswf

PHP_ARG_WITH(swf, for libswf support,
[  --with-swf[=DIR]        Include swf support])

if test "$PHP_SWF" != "no"; then
  if test -r $PHP_SWF/lib/libswf.a; then
    SWF_DIR=$PHP_SWF
  else
    AC_MSG_CHECKING(for libswf in default path)
    for i in /usr/local /usr; do
      if test -r $i/lib/libswf.a; then
        SWF_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$SWF_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libswf distribution -
   swf.h should be <swf-dir>/include and
   libswf.a should be in <swf-dir>/lib)
  fi

  AC_ADD_INCLUDE($SWF_DIR/include)

  PHP_SUBST(SWF_SHARED_LIBADD)
  AC_ADD_LIBRARY_WITH_PATH(swf, $SWF_DIR, SWF_SHARED_LIBADD)
  AC_DEFINE(HAVE_SWF,1,[ ])

  PHP_EXTENSION(swf, $ext_shared)
fi
