dnl $Id$
dnl config.m4 for extension Sablot

PHP_ARG_WITH(sablot, for Sablotron XSL support,
[  --with-sablot[=DIR]     Include Sablotron support])

PHP_ARG_WITH(expat-dir, libexpat dir for Sablotron 0.50,
[  --with-expat-dir=DIR    Sablotron: libexpat dir for Sablotron 0.50])

if test "$PHP_SABLOT" != "no"; then


  PHP_EXTENSION(sablot, $ext_shared)
  PHP_SUBST(SABLOT_SHARED_LIBADD)

  if test -r $PHP_SABLOT/include/sablot.h; then
    SABLOT_DIR=$PHP_SABLOT
  else
    AC_MSG_CHECKING(for Sablotron in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/sablot.h; then
        SABLOT_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$SABLOT_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the Sablotron distribution)
  fi

  testval=no
  for i in $PHP_EXPAT_DIR $SABLOT_DIR; do
    if test -f $i/lib/libexpat.a -o -f $i/lib/libexpat.s?; then
      AC_DEFINE(HAVE_LIBEXPAT2,1,[ ])
      PHP_ADD_LIBRARY_WITH_PATH(expat, $i/lib, SABLOT_SHARED_LIBADD)
      PHP_ADD_INCLUDE($i/include)
      AC_CHECK_LIB(sablot, SablotSetEncoding, AC_DEFINE(HAVE_SABLOT_SET_ENCODING,1,[ ]))
      testval=yes
    fi
  done

  if test "$testval" = "no"; then
    PHP_ADD_LIBRARY(xmlparse)
    PHP_ADD_LIBRARY(xmltok)
  fi

  PHP_ADD_INCLUDE($SABLOT_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH(sablot, $SABLOT_DIR/lib, SABLOT_SHARED_LIBADD)

  found_iconv=no
  AC_CHECK_LIB(c, iconv_open, found_iconv=yes)
  if test "$found_iconv" = "no"; then
      for i in /usr /usr/local $ICONV_DIR; do
        if test -f $i/lib/libconv.a -o -f $i/lib/libiconv.s?; then
          PHP_ADD_LIBRARY_WITH_PATH(iconv, $i/lib, SABLOT_SHARED_LIBADD)
          found_iconv=yes
        fi
      done
  fi
  
  if test "$found_iconv" = "no"; then
    AC_MSG_ERROR(iconv not found, in order to build sablotron you need the iconv library)
  fi
  
  AC_DEFINE(HAVE_SABLOT,1,[ ])
fi
