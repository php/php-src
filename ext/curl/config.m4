dnl $Id$
dnl config.m4 for extension CURL

PHP_ARG_WITH(curl, for CURL support,
[  --with-curl[=DIR]        Include CURL support])

if test "$PHP_CURL" != "no"; then
  if test -r $PHP_CURL/include/curl/easy.h; then
    CURL_DIR=$PHP_CURL
  else
    AC_MSG_CHECKING(for CURL in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/curl/easy.h; then
        CURL_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$CURL_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libcurl distribution -
    easy.h should be in <curl-dir>/include/curl/)
  fi

  AC_ADD_INCLUDE($CURL_DIR/include)

  PHP_SUBST(CURL_SHARED_LIBADD)
  AC_ADD_LIBRARY_WITH_PATH(curl, $CURL_DIR/lib, CURL_SHARED_LIBADD)

  AC_DEFINE(HAVE_CURL,1,[ ])

  PHP_EXTENSION(curl, $ext_shared)
fi