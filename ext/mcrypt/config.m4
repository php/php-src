dnl $Id$
dnl config.m4 for extension mcrypt
dnl don't forget to call PHP_EXTENSION(mcrypt)

PHP_ARG_WITH(mcrypt, for mcrypt support,
[  --with-mcrypt[=DIR]     Include mcrypt support.  DIR is the mcrypt
                          install directory.])

if test "$PHP_MCRYPT" != "no"; then
  for i in /usr/local /usr $PHP_MCRYPT; do
    if test -f $i/include/mcrypt.h; then
      MCRYPT_DIR=$i
    fi
  done

  if test -z "$MCRYPT_DIR"; then
    AC_MSG_ERROR(Please reinstall libmcrypt - I cannot find mcrypt.h)
  fi

  AC_ADD_INCLUDE($MCRYPT_DIR/include)
  PHP_SUBST(MCRYPT_SHARED_LIBADD)
  AC_CHECK_LIB(mcrypt, init_mcrypt, [ AC_DEFINE(HAVE_LIBMCRYPT22,1,[ ]) ])
  old_LIBS=$LIBS
  LIBS="$LIBS -lltdl"
  AC_CHECK_LIB(mcrypt, mcrypt_module_open, [LIBS="$LIBS -lltdl"],[ ],)
  LIBS=$old_LIBS
  AC_ADD_LIBRARY_WITH_PATH(mcrypt, $MCRYPT_DIR/lib, MCRYPT_SHARED_LIBADD)
  if test "$ac_cv_lib_mcrypt_init_mcrypt" = "yes"; then
	AC_DEFINE(HAVE_LIBMCRYPT22,1,[ ])
  fi
  if test "$ac_cv_lib_mcrypt_mcrypt_module_open" = "yes"; then
    AC_ADD_LIBRARY(ltdl)
	AC_DEFINE(HAVE_LIBMCRYPT24,1,[ ])
  fi
  AC_DEFINE(HAVE_LIBMCRYPT,1,[ ])

  PHP_EXTENSION(mcrypt, $ext_shared)
fi
