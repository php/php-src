dnl
dnl $Id$
dnl

PHP_ARG_WITH(iconv, for iconv support,
[  --with-iconv[=DIR]      Include iconv support])

if test "$PHP_ICONV" != "no"; then

  iconv_lib_name=iconv
  for i in $PHP_ICONV /usr/local /usr; do
    if test -r $i/include/giconv.h || test -r $i/include/iconv.h; then
      ICONV_DIR=$i
      if test -r $i/include/giconv.h; then
        iconv_lib_name=giconv
      fi
      break
    fi
  done

  if test -z "$ICONV_DIR"; then
    AC_MSG_ERROR(Please reinstall the iconv library.)
  fi
  
  if test -f $ICONV_DIR/lib/lib${iconv_lib_name}.a ||
     test -f $ICONV_DIR/lib/lib${iconv_lib_name}.$SHLIB_SUFFIX_NAME
  then
    PHP_ADD_LIBRARY_WITH_PATH($iconv_lib_name, $ICONV_DIR/lib, ICONV_SHARED_LIBADD)
    AC_CHECK_LIB($iconv_lib_name, libiconv_open, [
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
