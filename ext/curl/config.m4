dnl
dnl $Id$
dnl

PHP_ARG_WITH(curl, for CURL support,
[  --with-curl[=DIR]       Include CURL support])

dnl Temporary option while we develop this aspect of the extension
dnl PHP_ARG_WITH(curlwrappers, if we should use CURL for url streams,
dnl [  --with-curlwrappers     Use CURL for url streams], no, no)

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

  CURL_CONFIG="curl-config"
  AC_MSG_CHECKING(for cURL 7.9.8 or greater)

  if ${CURL_DIR}/bin/curl-config --libs print > /dev/null 2>&1; then
    CURL_CONFIG=${CURL_DIR}/bin/curl-config
  else
    if ${CURL_DIR}/curl-config --libs print > /dev/null 2>&1; then
       CURL_CONFIG=${CURL_DIR}/curl-config
    fi
  fi

  curl_version_full=`$CURL_CONFIG --version`
  curl_version=`echo ${curl_version_full} | sed -e 's/libcurl //' | awk 'BEGIN { FS = "."; } { printf "%d", ($1 * 1000 + $2) * 1000 + $3;}'`
  if test "$curl_version" -ge 7009008; then
    AC_MSG_RESULT($curl_version_full)
    CURL_LIBS=`$CURL_CONFIG --libs`
  else
    AC_MSG_ERROR(cURL version 7.9.8 or later is required to compile php with cURL support)
  fi

  PHP_ADD_INCLUDE($CURL_DIR/include)
  PHP_EVAL_LIBLINE($CURL_LIBS, CURL_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(curl, $CURL_DIR/lib, CURL_SHARED_LIBADD)

  PHP_CHECK_LIBRARY(curl,curl_easy_perform, 
  [ 
    AC_DEFINE(HAVE_CURL,1,[ ])
  ],[
    AC_MSG_ERROR(There is something wrong. Please check config.log for more information.)
  ],[
    $CURL_LIBS -L$CURL_DIR/lib
  ])

  PHP_CHECK_LIBRARY(curl,curl_version_info,
  [
    AC_DEFINE(HAVE_CURL_VERSION_INFO,1,[ ])
  ],[],[
    $CURL_LIBS -L$CURL_DIR/lib
  ])

  if test "$PHP_CURLWRAPPERS" != "no" ; then
	AC_DEFINE(PHP_CURL_URL_WRAPPERS,1,[ ])
  fi

  PHP_NEW_EXTENSION(curl, curl.c curlstreams.c, $ext_shared)
  PHP_SUBST(CURL_SHARED_LIBADD)
fi
