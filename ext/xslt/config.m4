dnl
dnl $Id$
dnl
dnl +------------------------------------------------------------------------------+
dnl |  This is where the magic of the extension reallly is.  Depending on what     |
dnl |  backend the user chooses, this script performs the magic                    |
dnl +------------------------------------------------------------------------------+

PHP_ARG_ENABLE(xslt, whether to enable xslt support,
[  --enable-xslt           Enable xslt support.])

PHP_ARG_WITH(xslt-sablot, for XSLT Sablotron backend,
[  --with-xslt-sablot=DIR    XSLT: Enable the sablotron backend.])

PHP_ARG_WITH(expat-dir, libexpat dir for Sablotron XSL support,
[  --with-expat-dir=DIR      XSLT: libexpat dir for Sablotron 0.50])

PHP_ARG_WITH(sablot-js, enable JavaScript for Sablotron,
[  --with-sablot-js=DIR    Sablotron: enable JavaScript support for Sablotron])

if test "$PHP_XSLT" != "no"; then

  PHP_NEW_EXTENSION(xslt, xslt.c sablot.c, $ext_shared)
  PHP_SUBST(XSLT_SHARED_LIBADD)

  if test "$PHP_XSLT_SABLOT" != "no"; then
    XSLT_CHECK_DIR=$PHP_XSLT_SABLOT
    XSLT_TEST_FILE=/include/sablot.h
    XSLT_BACKEND_NAME=Sablotron
    XSLT_LIBNAME=sablot
  fi

  if test -z "$XSLT_BACKEND_NAME"; then
    AC_MSG_ERROR(No backend specified for XSLT extension.)
  fi

  condition="$XSLT_CHECK_DIR$XSLT_TEST_FILE"

  if test -r $condition; then
    XSLT_DIR=$XSLT_CHECK_DIR
  else
    AC_MSG_CHECKING(for $XSLT_BACKEND_NAME libraries in the default path)
    for i in /usr /usr/local; do
      condition="$i$XSLT_TEST_FILE"
      if test -r $condition; then
        XSLT_DIR=$i
        AC_MSG_RESULT(found $XSLT_BACKEND_NAME in $i)
      fi
    done
  fi

  if test -z "$XSLT_DIR"; then
    AC_MSG_ERROR(not found. Please re-install the $XSLT_BACKEND_NAME distribution)
  fi
					
  if test "$PHP_XSLT_SABLOT" != "no"; then
    found_expat=no
    for i in $PHP_EXPAT_DIR $XSLT_DIR; do
      if test -f $i/lib/libexpat.a -o -f $i/lib/libexpat.so; then
        AC_DEFINE(HAVE_LIBEXPAT2, 1, [ ])
        PHP_ADD_INCLUDE($i/include)
        PHP_ADD_LIBRARY_WITH_PATH(expat, $i/lib, XSLT_SHARED_LIBADD)
        found_expat=yes
      fi
    done

    if test "$found_expat" = "no"; then
      PHP_ADD_LIBRARY(xmlparse)
      PHP_ADD_LIBRARY(xmltok)
    fi

    found_iconv=no

    if test -z "$ICONV_DIR"; then
      AC_MSG_ERROR(Please specify the location of iconv with --with-iconv)
    fi
  
    if test -f $ICONV_DIR/lib/lib${iconv_lib_name}.a ||
       test -f $ICONV_DIR/lib/lib${iconv_lib_name}.$SHLIB_SUFFIX_NAME
    then
      PHP_ADD_LIBRARY_WITH_PATH($iconv_lib_name, $ICONV_DIR/lib, XSLT_SHARED_LIBADD)
      AC_CHECK_LIB($iconv_lib_name, libiconv_open, [
        found_iconv=yes
      ])
    else
      AC_CHECK_LIB(c, iconv_open, [
        found_iconv=yes
      ])
    fi

    if test "$found_iconv" = "no"; then
      AC_MSG_ERROR([iconv not found, in order to build sablotron you need the iconv library])
    fi
 
    if test "$PHP_SABLOT_JS" != "no"; then
      found_js=no
      AC_CHECK_LIB(js, JS_GetRuntime, found_js=yes)
      if test "$found_js" = "yes"; then
        PHP_ADD_LIBRARY(js)
      fi
    fi

    AC_DEFINE(HAVE_SABLOT_BACKEND, 1, [ ])
    if test "$found_expat" = "yes"; then
     old_LIBS=$LIBS
     LIBS="$LIBS -lexpat"
     AC_CHECK_LIB(sablot, SablotSetEncoding, AC_DEFINE(HAVE_SABLOT_SET_ENCODING, 1, [ ]))
     LIBS=$old_LIBS
    fi
  fi

  PHP_ADD_INCLUDE($XSLT_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH($XSLT_LIBNAME, $XSLT_DIR/lib, XSLT_SHARED_LIBADD)

  AC_DEFINE(HAVE_XSLT, 1, [ ])
fi
