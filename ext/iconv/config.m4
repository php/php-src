dnl
dnl $Id$
dnl

PHP_ARG_WITH(iconv, for iconv support,
[  --with-iconv[=DIR]      Include iconv support])

if test "$PHP_ICONV" != "no"; then

dnl This is a fix for why FreeBSD does not work with ICONV
dnl It seems libtool checks for libiconv_open which only exists in
dnl the giconv series of files under FreeBSD

  ac_os_uname=`uname -s 2>/dev/null`

  if test "$ac_os_uname" = "FreeBSD"; then
	lib_name=giconv
  else
	lib_name=iconv
  fi

  for i in /usr /usr/local $PHP_ICONV; do
    test -r $i/include/${lib_name}.h && ICONV_DIR=$i
  done

  if test -z "$ICONV_DIR"; then
    AC_MSG_ERROR(Please reinstall the iconv library.)
  fi
  
  if test -f $ICONV_DIR/lib/libiconv.a -o -f $ICONV_DIR/lib/lib${lib_name}.$SHLIB_SUFFIX_NAME ; then
    PHP_ADD_LIBRARY_WITH_PATH(iconv, $ICONV_DIR/lib, ICONV_SHARED_LIBADD)
    AC_CHECK_LIB(iconv, libiconv_open, [
    	AC_DEFINE(HAVE_ICONV, 1, [ ])
    	AC_DEFINE(HAVE_LIBICONV, 1, [ ])
    ])
  else
    AC_CHECK_LIB(c, iconv_open, AC_DEFINE(HAVE_ICONV, 1, [ ]))
  fi

  PHP_ADD_INCLUDE($ICONV_DIR/include)
  PHP_EXTENSION(iconv, $ext_shared)
  PHP_SUBST(ICONV_SHARED_LIBADD)
fi
