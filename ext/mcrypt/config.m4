dnl
dnl $Id$
dnl 

AC_DEFUN(PHP_MCRYPT_CHECK_VERSION,[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=-I$MCRYPT_DIR/include
  AC_MSG_CHECKING(for libmcrypt version)
  AC_EGREP_CPP(yes,[
#include <mcrypt.h>
#if MCRYPT_API_VERSION >= 20020309
  yes
#endif
  ],[
    AC_MSG_RESULT(>= 2.5.3)
  ],[
    AC_MSG_ERROR(libmcrypt version 2.5.3 or greater required.)
  ])
  CPPFLAGS=$old_CPPFLAGS
])  


PHP_ARG_WITH(mcrypt, for mcrypt support,
[  --with-mcrypt[=DIR]     Include mcrypt support.])

if test "$PHP_MCRYPT" != "no"; then
  for i in /usr/local /usr $PHP_MCRYPT; do
    if test -f $i/include/mcrypt.h; then
      MCRYPT_DIR=$i
    fi
  done

  if test -z "$MCRYPT_DIR"; then
    AC_MSG_ERROR(mcrypt.h not found. Please reinstall libmcrypt.)
  fi

  PHP_MCRYPT_CHECK_VERSION

  PHP_ADD_LIBRARY(ltdl,, MCRYPT_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(mcrypt, $MCRYPT_DIR/lib, MCRYPT_SHARED_LIBADD)
  PHP_ADD_INCLUDE($MCRYPT_DIR/include)
  AC_DEFINE(HAVE_LIBMCRYPT,1,[ ])

  PHP_SUBST(MCRYPT_SHARED_LIBADD)
  PHP_NEW_EXTENSION(mcrypt, mcrypt.c, $ext_shared)
fi
