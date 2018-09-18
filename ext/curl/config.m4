dnl config.m4 for extension curl

PHP_ARG_WITH(curl, for cURL support,
[  --with-curl[=DIR]         Include cURL support])

if test "$PHP_CURL" != "no"; then
  if test -z "$PKG_CONFIG"; then
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  fi

  if test -x "$PKG_CONFIG"; then
    dnl using pkg-config output

    AC_MSG_CHECKING(for libcurl.pc)
    if test "$PHP_CURL" = "yes" -o "$PHP_CURL" = "/usr"; then
      PKNAME=libcurl
      AC_MSG_RESULT(using default path)
    elif test -r $PHP_CURL/$PHP_LIBDIR/pkgconfig/libcurl.pc; then
      PKNAME=$PHP_CURL/$PHP_LIBDIR/pkgconfig/libcurl.pc
      AC_MSG_RESULT(using $PKNAME)
    elif test -r $PHP_CURL/lib/pkgconfig/libcurl.pc; then
      PKNAME=$PHP_CURL/lib/pkgconfig/libcurl.pc
      AC_MSG_RESULT(using $PKNAME)
    else
      AC_MSG_RESULT(not found)
      AC_MSG_WARN(Could not find libcurl.pc. Try without $PHP_CURL or set PKG_CONFIG_PATH)
    fi
  fi

  if test -n "$PKNAME"; then
    AC_MSG_CHECKING(for cURL 7.15.5 or greater)
    if $PKG_CONFIG --atleast-version 7.15.5 $PKNAME; then
      curl_version_full=`$PKG_CONFIG --modversion $PKNAME`
      AC_MSG_RESULT($curl_version_full)
    else
      AC_MSG_ERROR(cURL version 7.15.5 or later is required to compile php with cURL support)
    fi

    CURL_LIBS=`$PKG_CONFIG --libs   $PKNAME`
    CURL_INCL=`$PKG_CONFIG --cflags $PKNAME`
    CURL_SSL=`$PKG_CONFIG --variable=supported_features $PKNAME| $EGREP SSL`
  else
    dnl fallback to old vay, using curl-config
    AC_MSG_WARN(Fallback: search for curl headers and curl-config)

    if test -r $PHP_CURL/include/curl/easy.h; then
      CURL_DIR=$PHP_CURL
    else
      AC_MSG_CHECKING(for cURL in default path)
      for i in /usr/local /usr; do
        if test -r $i/include/curl/easy.h; then
          CURL_DIR=$i
          AC_MSG_RESULT(found in $i)
          break
        fi
      done
    fi

    if test -z "$CURL_DIR"; then
      AC_MSG_RESULT(not found)
      AC_MSG_ERROR(Please reinstall the libcurl distribution -
      easy.h should be in <curl-dir>/include/curl/)
    fi

    CURL_CONFIG="curl-config"
    AC_MSG_CHECKING(for cURL 7.15.5 or greater)

    if ${CURL_DIR}/bin/curl-config --libs > /dev/null 2>&1; then
      CURL_CONFIG=${CURL_DIR}/bin/curl-config
    else
      if ${CURL_DIR}/curl-config --libs > /dev/null 2>&1; then
        CURL_CONFIG=${CURL_DIR}/curl-config
      fi
    fi

    curl_version_full=`$CURL_CONFIG --version`
    curl_version=`echo ${curl_version_full} | sed -e 's/libcurl //' | $AWK 'BEGIN { FS = "."; } { printf "%d", ($1 * 1000 + $2) * 1000 + $3;}'`
    if test "$curl_version" -ge 7015005; then
      AC_MSG_RESULT($curl_version_full)
      CURL_LIBS=`$CURL_CONFIG --libs`
      CURL_INCL=`$CURL_CONFIG --cflags`
      CURL_SSL=`$CURL_CONFIG --feature | $EGREP SSL`
    else
      AC_MSG_ERROR(cURL version 7.15.5 or later is required to compile php with cURL support)
    fi
  fi

  dnl common stuff (pkg-config / curl-config)

  PHP_EVAL_LIBLINE($CURL_LIBS, CURL_SHARED_LIBADD)
  PHP_EVAL_INCLINE($CURL_INCL, CURL_SHARED_LIBADD)

  AC_MSG_CHECKING([for SSL support in libcurl])
  if test -n "$CURL_SSL"; then
    AC_MSG_RESULT([yes])
    AC_DEFINE([HAVE_CURL_SSL], [1], [Have cURL with  SSL support])

    save_CFLAGS="$CFLAGS"
    CFLAGS=$CURL_INCL
    save_LDFLAGS="$LDFLAGS"
    LDFLAGS=$CURL_LIBS

    AC_PROG_CPP
    AC_MSG_CHECKING([for openssl support in libcurl])
    AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <strings.h>
#include <curl/curl.h>

int main(int argc, char *argv[])
{
  curl_version_info_data *data = curl_version_info(CURLVERSION_NOW);

  if (data && data->ssl_version && *data->ssl_version) {
    const char *ptr = data->ssl_version;

    while(*ptr == ' ') ++ptr;
    return strncasecmp(ptr, "OpenSSL", sizeof("OpenSSL")-1);
  }
  return 1;
}
    ]])],[
      AC_MSG_RESULT([yes])
      AC_CHECK_HEADERS([openssl/crypto.h], [
        AC_DEFINE([HAVE_CURL_OPENSSL], [1], [Have cURL with OpenSSL support])
      ])
    ], [
      AC_MSG_RESULT([no])
    ], [
      AC_MSG_RESULT([no])
    ])

    AC_MSG_CHECKING([for gnutls support in libcurl])
    AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <strings.h>
#include <curl/curl.h>

int main(int argc, char *argv[])
{
  curl_version_info_data *data = curl_version_info(CURLVERSION_NOW);

  if (data && data->ssl_version && *data->ssl_version) {
    const char *ptr = data->ssl_version;

    while(*ptr == ' ') ++ptr;
    return strncasecmp(ptr, "GnuTLS", sizeof("GnuTLS")-1);
  }
  return 1;
}
]])], [
      AC_MSG_RESULT([yes])
      AC_CHECK_HEADER([gcrypt.h], [
        AC_DEFINE([HAVE_CURL_GNUTLS], [1], [Have cURL with GnuTLS support])
      ])
    ], [
      AC_MSG_RESULT([no])
    ], [
      AC_MSG_RESULT([no])
    ])

    CFLAGS="$save_CFLAGS"
    LDFLAGS="$save_LDFLAGS"
  else
    AC_MSG_RESULT([no])
  fi

  PHP_CHECK_LIBRARY(curl,curl_easy_perform,
  [
    AC_DEFINE(HAVE_CURL,1,[ ])
  ],[
    AC_MSG_ERROR(There is something wrong. Please check config.log for more information.)
  ],[
    $CURL_LIBS
  ])

  PHP_CHECK_LIBRARY(curl,curl_easy_strerror,
  [
    AC_DEFINE(HAVE_CURL_EASY_STRERROR,1,[ ])
  ],[],[
    $CURL_LIBS
  ])

  PHP_CHECK_LIBRARY(curl,curl_multi_strerror,
  [
    AC_DEFINE(HAVE_CURL_MULTI_STRERROR,1,[ ])
  ],[],[
    $CURL_LIBS
  ])

  PHP_NEW_EXTENSION(curl, interface.c multi.c share.c curl_file.c, $ext_shared)
  PHP_SUBST(CURL_SHARED_LIBADD)
fi
