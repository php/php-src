PHP_ARG_WITH([curl],
  [for cURL support],
  [AS_HELP_STRING([--with-curl],
    [Include cURL support])])

if test "$PHP_CURL" != "no"; then
  PKG_CHECK_MODULES([CURL], [libcurl >= 7.61.0])
  PKG_CHECK_VAR([CURL_FEATURES], [libcurl], [supported_features])

  PHP_EVAL_LIBLINE([$CURL_LIBS], [CURL_SHARED_LIBADD])
  PHP_EVAL_INCLINE([$CURL_CFLAGS])

  AC_MSG_CHECKING([for SSL support in libcurl])
  AS_CASE([$CURL_FEATURES], [*SSL*], [CURL_SSL=yes], [CURL_SSL=no])
  AC_MSG_RESULT([$CURL_SSL])

  AS_IF([test "x$PHP_THREAD_SAFETY" = xyes && test "x$CURL_SSL" = xyes],
    [AC_CACHE_CHECK([whether libcurl is linked against old OpenSSL < 1.1],
      [php_cv_lib_curl_ssl], [
      save_LIBS=$LIBS
      save_CFLAGS=$CFLAGS
      LIBS="$LIBS $CURL_SHARED_LIBADD"
      CFLAGS="$CFLAGS $CURL_CFLAGS"

      AC_RUN_IFELSE([AC_LANG_PROGRAM([
#include <stdio.h>
#include <strings.h>
#include <curl/curl.h>
], [
  curl_version_info_data *data = curl_version_info(CURLVERSION_NOW);

  if (data && data->ssl_version && *data->ssl_version) {
    const char *ptr = data->ssl_version;

    while(*ptr == ' ') ++ptr;
    int major, minor;
    if (sscanf(ptr, "OpenSSL/%d", &major) == 1) {
      if (major >= 3) {
        /* OpenSSL version 3 or later */
        return 4;
      }
    }
    if (sscanf(ptr, "OpenSSL/%d.%d", &major, &minor) == 2) {
      if (major > 1 || (major == 1 && minor >= 1)) {
        /* OpenSSL version 1.1 or later */
        return 3;
      }
    }
    if (strncasecmp(ptr, "OpenSSL", sizeof("OpenSSL")-1) == 0) {
      /* Old OpenSSL version */
      return 0;
    }
    /* Different SSL library */
    return 2;
  }
  /* No SSL support */
  return 1;
])],
      [php_cv_lib_curl_ssl=yes],
      [php_cv_lib_curl_ssl=no],
      [php_cv_lib_curl_ssl=no])
      LIBS=$save_LIBS
      CFLAGS=$save_CFLAGS
    ])

    AS_VAR_IF([php_cv_lib_curl_ssl], [yes], [
      AC_DEFINE([HAVE_CURL_OLD_OPENSSL], [1],
        [Define to 1 if libcurl is linked against old OpenSSL < 1.1.])
      PHP_SETUP_OPENSSL([CURL_SHARED_LIBADD],
        [AC_CHECK_HEADERS([openssl/crypto.h])])
    ])
  ])

  PHP_CHECK_LIBRARY([curl],
    [curl_easy_perform],
    [AC_DEFINE([HAVE_CURL], [1],
      [Define to 1 if the PHP extension 'curl' is available.])],
    [AC_MSG_FAILURE([The libcurl check failed.])],
    [$CURL_LIBS])

  PHP_NEW_EXTENSION([curl],
    [interface.c multi.c share.c curl_file.c],
    [$ext_shared])
  PHP_INSTALL_HEADERS([ext/curl], [php_curl.h])
  PHP_SUBST([CURL_SHARED_LIBADD])
fi
