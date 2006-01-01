dnl $Id$
dnl config.m4 for extension phar

PHP_ARG_ENABLE(phar, for phar support/phar zlib support,
[  --enable-phar           Enable phar support, use --with-zlib-dir if zlib detection fails])

if test -z "$PHP_ZLIB_DIR"; then
  PHP_ARG_WITH(zlib-dir, for the location of libz, 
  [  --with-zlib-dir[=DIR]     PHAR: Set the path to libz install prefix], no, no)
fi

if test "$PHP_PHAR" != "no"; then
  PHP_NEW_EXTENSION(phar, phar.c, $ext_shared)
  PHP_SUBST(ZLIB_SHARED_LIBADD)

  if test "$PHP_ZLIB_DIR" != "no"; then
    PHP_CHECK_LIBRARY(z, gzgets, [
      AC_DEFINE(HAVE_PHAR_ZLIB,1,[ ]) 
      AC_MSG_CHECKING(whether phar uses zlib)
      AC_MSG_RESULT(yes)
      PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR, PHAR_SHARED_LIBADD)
      AC_DEFINE(HAVE_PHAR_ZLIB,1,[ ]) 
    ],[
      AC_MSG_CHECKING(whether phar uses zlib)
      AC_MSG_RESULT(no, PHAR extension requires zlib >= 1.0.9)
    ],[
      $ac_extra
    ])
  fi
fi
