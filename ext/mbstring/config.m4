AC_DEFUN([PHP_MBSTRING_ADD_SOURCES], [
  PHP_MBSTRING_SOURCES="$PHP_MBSTRING_SOURCES $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_BASE_SOURCES], [
  PHP_MBSTRING_BASE_SOURCES="$PHP_MBSTRING_BASE_SOURCES $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_BUILD_DIR], [
  PHP_MBSTRING_EXTRA_BUILD_DIRS="$PHP_MBSTRING_EXTRA_BUILD_DIRS $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_INCLUDE], [
  PHP_MBSTRING_EXTRA_INCLUDES="$PHP_MBSTRING_EXTRA_INCLUDES $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_CFLAG], [
  PHP_MBSTRING_CFLAGS="$PHP_MBSTRING_CFLAGS $1"
])

AC_DEFUN([PHP_MBSTRING_SETUP_MBREGEX], [
  PKG_CHECK_MODULES([ONIG], [oniguruma])
  PHP_EVAL_LIBLINE([$ONIG_LIBS], [MBSTRING_SHARED_LIBADD])
  PHP_EVAL_INCLINE([$ONIG_CFLAGS])

  AC_CACHE_CHECK([if oniguruma has an invalid entry for KOI8 encoding],
    [php_cv_lib_onig_invalid_koi8],
    [save_old_LIBS=$LIBS
      LIBS="$LIBS $MBSTRING_SHARED_LIBADD"
      save_old_CFLAGS=$CFLAGS
      CFLAGS="$CFLAGS $ONIG_CFLAGS"
      AC_LINK_IFELSE([AC_LANG_PROGRAM([
        #include <stdint.h>
        #include <oniguruma.h>
      ],
      [return (intptr_t)(ONIG_ENCODING_KOI8 + 1);])],
      [php_cv_lib_onig_invalid_koi8=no],
      [php_cv_lib_onig_invalid_koi8=yes])
      LIBS=$save_old_LIBS
      CFLAGS=$save_old_CFLAGS])
  AS_VAR_IF([php_cv_lib_onig_invalid_koi8], [yes],
    [AC_DEFINE([PHP_ONIG_BAD_KOI8_ENTRY], [1],
      [Define to 1 if oniguruma has an invalid entry for KOI8 encoding.])])

  PHP_MBSTRING_ADD_CFLAG([-DONIG_ESCAPE_UCHAR_COLLISION=1])
  PHP_MBSTRING_ADD_CFLAG([-DUChar=OnigUChar])

  AC_DEFINE([HAVE_MBREGEX], [1],
    [Define to 1 if mbstring has multibyte regex support enabled.])

  PHP_MBSTRING_ADD_BASE_SOURCES([php_mbregex.c])
  PHP_INSTALL_HEADERS([ext/mbstring], [php_mbregex.h php_onig_compat.h])
])

AC_DEFUN([PHP_MBSTRING_SETUP_LIBMBFL], [
  dnl
  dnl Bundled libmbfl is required and can not be disabled
  dnl
  PHP_MBSTRING_ADD_BUILD_DIR([libmbfl])
  PHP_MBSTRING_ADD_BUILD_DIR([libmbfl/mbfl])
  PHP_MBSTRING_ADD_BUILD_DIR([libmbfl/filters])
  PHP_MBSTRING_ADD_BUILD_DIR([libmbfl/nls])
  PHP_MBSTRING_ADD_INCLUDE([libmbfl])
  PHP_MBSTRING_ADD_INCLUDE([libmbfl/mbfl])

  PHP_MBSTRING_ADD_SOURCES([
    libmbfl/filters/html_entities.c
    libmbfl/filters/mbfilter_7bit.c
    libmbfl/filters/mbfilter_base64.c
    libmbfl/filters/mbfilter_cjk.c
    libmbfl/filters/mbfilter_htmlent.c
    libmbfl/filters/mbfilter_qprint.c
    libmbfl/filters/mbfilter_singlebyte.c
    libmbfl/filters/mbfilter_ucs2.c
    libmbfl/filters/mbfilter_ucs4.c
    libmbfl/filters/mbfilter_utf16.c
    libmbfl/filters/mbfilter_utf32.c
    libmbfl/filters/mbfilter_utf7.c
    libmbfl/filters/mbfilter_utf7imap.c
    libmbfl/filters/mbfilter_utf8.c
    libmbfl/filters/mbfilter_uuencode.c
    libmbfl/mbfl/mbfilter.c
    libmbfl/mbfl/mbfilter_8bit.c
    libmbfl/mbfl/mbfilter_pass.c
    libmbfl/mbfl/mbfilter_wchar.c
    libmbfl/mbfl/mbfl_convert.c
    libmbfl/mbfl/mbfl_encoding.c
    libmbfl/mbfl/mbfl_filter_output.c
    libmbfl/mbfl/mbfl_language.c
    libmbfl/mbfl/mbfl_memory_device.c
    libmbfl/mbfl/mbfl_string.c
    libmbfl/nls/nls_de.c
    libmbfl/nls/nls_en.c
    libmbfl/nls/nls_ja.c
    libmbfl/nls/nls_kr.c
    libmbfl/nls/nls_neutral.c
    libmbfl/nls/nls_ru.c
    libmbfl/nls/nls_uni.c
    libmbfl/nls/nls_zh.c
    libmbfl/nls/nls_hy.c
    libmbfl/nls/nls_tr.c
    libmbfl/nls/nls_ua.c
  ])

  PHP_INSTALL_HEADERS([ext/mbstring], m4_normalize([
    libmbfl/config.h
    libmbfl/mbfl/eaw_table.h
    libmbfl/mbfl/mbfilter_8bit.h
    libmbfl/mbfl/mbfilter_pass.h
    libmbfl/mbfl/mbfilter_wchar.h
    libmbfl/mbfl/mbfilter.h
    libmbfl/mbfl/mbfl_consts.h
    libmbfl/mbfl/mbfl_convert.h
    libmbfl/mbfl/mbfl_defs.h
    libmbfl/mbfl/mbfl_encoding.h
    libmbfl/mbfl/mbfl_filter_output.h
    libmbfl/mbfl/mbfl_language.h
    libmbfl/mbfl/mbfl_memory_device.h
    libmbfl/mbfl/mbfl_string.h
  ]))
])

dnl
dnl Main config
dnl

PHP_ARG_ENABLE([mbstring],
  [whether to enable multibyte string support],
  [AS_HELP_STRING([--enable-mbstring],
    [Enable multibyte string support])])

PHP_ARG_ENABLE([mbregex],
  [whether to enable multibyte regex support (requires oniguruma)],
  [AS_HELP_STRING([--disable-mbregex],
    [MBSTRING: Disable multibyte regex support])],
  [yes],
  [no])

if test "$PHP_MBSTRING" != "no"; then
  AC_DEFINE([HAVE_MBSTRING], [1],
    [Define to 1 if the PHP extension 'mbstring' is available.])

  PHP_MBSTRING_ADD_BASE_SOURCES([mbstring.c php_unicode.c mb_gpc.c])

  AS_VAR_IF([PHP_MBREGEX], [yes], [PHP_MBSTRING_SETUP_MBREGEX])

  dnl libmbfl is required
  PHP_MBSTRING_SETUP_LIBMBFL

  PHP_NEW_EXTENSION([mbstring],
    [$PHP_MBSTRING_BASE_SOURCES $PHP_MBSTRING_SOURCES],
    [$ext_shared],,
    [$PHP_MBSTRING_CFLAGS -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])

  PHP_SUBST([MBSTRING_SHARED_LIBADD])

  for dir in $PHP_MBSTRING_EXTRA_BUILD_DIRS; do
    PHP_ADD_BUILD_DIR([$ext_builddir/$dir], [1])
  done

  for dir in $PHP_MBSTRING_EXTRA_INCLUDES; do
    PHP_ADD_INCLUDE([$ext_srcdir/$dir])
    PHP_ADD_INCLUDE([$ext_builddir/$dir])
  done

  out="php_config.h"

  if test "$ext_shared" != "no" && test -f "$ext_builddir/config.h.in"; then
    out="$abs_builddir/config.h"
  fi

  cat > $ext_builddir/libmbfl/config.h <<EOF
#include "$out"
EOF

  PHP_INSTALL_HEADERS([ext/mbstring], [mbstring.h])

  PHP_ADD_EXTENSION_DEP(mbstring, pcre)
fi
