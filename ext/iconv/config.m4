dnl
dnl $Id$
dnl

PHP_ARG_WITH(iconv, for iconv support,
[  --with-iconv[=DIR]      Include iconv support])

if test "$PHP_ICONV" != "no"; then

  PHP_SETUP_ICONV(ICONV_SHARED_LIBADD, [
    PHP_NEW_EXTENSION(iconv, iconv.c, $ext_shared)
    PHP_SUBST(ICONV_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR(Please reinstall the iconv library.)
  ])

fi
