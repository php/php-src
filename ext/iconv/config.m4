PHP_ARG_WITH([iconv],
  [for iconv support],
  [AS_HELP_STRING([[--without-iconv[=DIR]]],
    [Exclude iconv support])],
  [yes])

if test "$PHP_ICONV" != "no"; then
  PHP_SETUP_ICONV([ICONV_SHARED_LIBADD],,
    [AC_MSG_FAILURE([The iconv not found.])])

  save_LIBS=$LIBS
  save_CFLAGS=$CFLAGS
  LIBS="$LIBS $ICONV_SHARED_LIBADD"
  CFLAGS="$INCLUDES $CFLAGS"

  AC_CACHE_CHECK([for iconv implementation], [php_cv_iconv_implementation], [
    AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <gnu/libc-version.h>],
      [gnu_get_libc_version();])],
      [php_cv_iconv_implementation=glibc],
      [php_cv_iconv_implementation=unknown])

    AS_VAR_IF([php_cv_iconv_implementation], [unknown],
      [AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <iconv.h>],
        [(void) _libiconv_version])],
        [php_cv_iconv_implementation="GNU libiconv"],
        [php_cv_iconv_implementation=unknown])])

    dnl BSD iconv by Konstantin Chuguev.
    AS_VAR_IF([php_cv_iconv_implementation], [unknown],
      [AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <iconv.h>],
        [iconv_ccs_init(NULL, NULL);])],
        [php_cv_iconv_implementation=BSD],
        [php_cv_iconv_implementation=unknown])])

    AS_VAR_IF([php_cv_iconv_implementation], [unknown],
      [AC_LINK_IFELSE([AC_LANG_PROGRAM([#include <iconv.h>],
        [cstoccsid("");])],
        [php_cv_iconv_implementation=IBM],
        [php_cv_iconv_implementation=unknown])])
  ])

  AH_TEMPLATE([PHP_ICONV_IMPL], [The iconv implementation.])

  AS_CASE([$php_cv_iconv_implementation],
    [glibc], [
      AC_DEFINE([HAVE_GLIBC_ICONV], [1],
        [Define to 1 if iconv implementation is glibc.])
      AC_DEFINE([PHP_ICONV_IMPL], ["glibc"])
    ],
    ["GNU libiconv"], [
      AC_DEFINE([PHP_ICONV_IMPL], ["libiconv"])
      AC_DEFINE([HAVE_LIBICONV], [1])
    ],
    [BSD], [AC_DEFINE([PHP_ICONV_IMPL], ["BSD iconv"])],
    [IBM], [
      AC_DEFINE([HAVE_IBM_ICONV], [1],
        [Define to 1 if iconv implementation is IBM.])
      AC_DEFINE([PHP_ICONV_IMPL], ["IBM iconv"])
    ])

  AC_CACHE_CHECK([if iconv supports errno], [php_cv_iconv_errno],
    [AC_RUN_IFELSE([AC_LANG_SOURCE([
#include <iconv.h>
#include <errno.h>

int main(void) {
  iconv_t cd;
  cd = iconv_open( "*blahblah*", "*blahblahblah*" );
  if (cd == (iconv_t)(-1)) {
    if (errno == EINVAL) {
      return 0;
  } else {
      return 1;
    }
  }
  iconv_close( cd );
  return 2;
}
    ])],
    [php_cv_iconv_errno=yes],
    [php_cv_iconv_errno=no],
    [php_cv_iconv_errno=yes])])
  AS_VAR_IF([php_cv_iconv_errno], [yes],,
    [AC_MSG_FAILURE([The iconv check failed, 'errno' is missing.])])

  AC_CACHE_CHECK([if iconv supports //IGNORE], [php_cv_iconv_ignore],
    [AC_RUN_IFELSE([AC_LANG_SOURCE([
#include <iconv.h>
#include <stdlib.h>

int main(void) {
  iconv_t cd = iconv_open( "UTF-8//IGNORE", "UTF-8" );
  if(cd == (iconv_t)-1) {
    return 1;
  }
  char *in_p = "\xC3\xC3\xC3\xB8";
  size_t in_left = 4, out_left = 4096;
  char *out = malloc(out_left);
  char *out_p = out;
  size_t result = iconv(cd, (char **) &in_p, &in_left, (char **) &out_p, &out_left);
  if(result == (size_t)-1) {
    return 1;
  }
  return 0;
}
    ])],
    [php_cv_iconv_ignore=yes],
    [php_cv_iconv_ignore=no],
    [php_cv_iconv_ignore=no])])
  AS_VAR_IF([php_cv_iconv_ignore], [no],
    [AC_DEFINE([ICONV_BROKEN_IGNORE], [1],
      [Define to 1 if iconv has broken IGNORE.])])

  LIBS=$save_LIBS
  CFLAGS=$save_CFLAGS

  AC_DEFINE([HAVE_ICONV], [1],
    [Define to 1 if the PHP extension 'iconv' is available.])

  PHP_NEW_EXTENSION([iconv],
    [iconv.c],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
  PHP_SUBST([ICONV_SHARED_LIBADD])
  PHP_INSTALL_HEADERS([ext/iconv], [php_iconv.h])
fi
