dnl
dnl $Id$
dnl
dnl +------------------------------------------------------------------------------+
dnl |  This is where the magic of the extension reallly is.  Depending on what     |
dnl |  backend the user chooses, this script performs the magic                    |
dnl +------------------------------------------------------------------------------+

PHP_ARG_ENABLE(xslt, whether to enable xslt support,
[  --enable-xslt           Enable xslt support.])

xslt_ext_shared=$ext_shared

PHP_ARG_WITH(xslt-sablot, for XSLT Sablotron backend,
[  --with-xslt-sablot=DIR    XSLT: Enable the sablotron backend.])

PHP_ARG_WITH(expat-dir, for libexpat dir for Sablotron XSL support,
[  --with-expat-dir=DIR      XSLT: libexpat dir for Sablotron.])

PHP_ARG_WITH(iconv-dir, for iconv dir for Sablotron XSL support,
[  --with-iconv-dir=DIR      XSLT: iconv dir for Sablotron.])

dnl This configure option is optional. 
AC_MSG_CHECKING([for JavaScript for Sablotron XSL support])
AC_ARG_WITH(sablot-js,
[  --with-sablot-js=DIR      XSLT: enable JavaScript support for Sablotron.],[
  PHP_SABLOT_JS=$withval
  AC_MSG_RESULT(yes)
], [
  PHP_SABLOT_JS=no
  AC_MSG_RESULT(no)
])

if test "$PHP_XSLT" != "no"; then

  PHP_NEW_EXTENSION(xslt, xslt.c sablot.c, $xslt_ext_shared)
  PHP_SUBST(XSLT_SHARED_LIBADD)

  if test "$PHP_XSLT_SABLOT" != "no"; then
    XSLT_CHECK_DIR=$PHP_XSLT_SABLOT
    XSLT_TEST_FILE=/include/sablot.h
    XSLT_BACKEND_NAME=Sablotron
    XSLT_LIBNAME=sablot
  fi

  if test -z "$XSLT_BACKEND_NAME"; then
    AC_MSG_ERROR([No backend specified for XSLT extension.])
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
    AC_MSG_ERROR([not found. Please re-install the $XSLT_BACKEND_NAME distribution.])
  fi
					
  if test "$PHP_XSLT_SABLOT" != "no"; then
    found_expat=no
    for i in $PHP_EXPAT_DIR $XSLT_DIR /usr/local /usr; do
      if test -f $i/lib/libexpat.a -o -f $i/lib/libexpat.$SHLIB_SUFFIX_NAME; then
        AC_DEFINE(HAVE_LIBEXPAT2, 1, [ ])
        PHP_ADD_INCLUDE($i/include)
        PHP_ADD_LIBRARY_WITH_PATH(expat, $i/lib, XSLT_SHARED_LIBADD)
        found_expat=yes
        break
      fi
    done

    if test "$found_expat" = "no"; then
      AC_MSG_ERROR([expat not found. To build sablotron you need the expat library.])
    fi

    if test "$PHP_ICONV_DIR" != "no"; then
      PHP_ICONV=$PHP_ICONV_DIR
    fi

    if test "$PHP_ICONV" = "no"; then
      PHP_ICONV=yes
    fi

    PHP_SETUP_ICONV(XSLT_SHARED_LIBADD, [], [
      AC_MSG_ERROR([iconv not found. To build sablotron you need the iconv library.])
    ])
     
    if test "$PHP_SABLOT_JS" != "no"; then
      for i in /usr/local /usr $PHP_SABLOT_JS; do
        if test -f $i/lib/libjs.a -o -f $i/lib/libjs.$SHLIB_SUFFIX_NAME; then
          PHP_SABLOT_JS_DIR=$i
        fi
      done

      PHP_CHECK_LIBRARY(js, JS_GetRuntime,
      [
        PHP_ADD_LIBRARY_WITH_PATH(js, $PHP_SABLOT_JS_DIR/lib, XSLT_SHARED_LIBADD)
      ], [
        AC_MSG_ERROR([libjs not found. Please check config.log for more information.])
      ], [
        -L$PHP_SABLOT_JS_DIR/lib
      ])
    fi

    PHP_CHECK_LIBRARY(sablot, SablotSetEncoding,
    [
      AC_DEFINE(HAVE_SABLOT_SET_ENCODING, 1, [ ])
    ], [], [
      -L$XSLT_DIR/lib
    ])

    AC_DEFINE(HAVE_SABLOT_BACKEND, 1, [ ])
  fi

  PHP_ADD_INCLUDE($XSLT_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH($XSLT_LIBNAME, $XSLT_DIR/lib, XSLT_SHARED_LIBADD)

  AC_DEFINE(HAVE_XSLT, 1, [ ])
fi
