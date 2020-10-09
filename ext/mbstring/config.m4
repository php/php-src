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

AC_DEFUN([PHP_MBSTRING_ADD_INSTALL_HEADERS], [
  PHP_MBSTRING_INSTALL_HEADERS="$PHP_MBSTRING_INSTALL_HEADERS $1"
])

AC_DEFUN([PHP_MBSTRING_EXTENSION], [
  PHP_NEW_EXTENSION(mbstring, $PHP_MBSTRING_BASE_SOURCES $PHP_MBSTRING_SOURCES, $ext_shared,, $PHP_MBSTRING_CFLAGS -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(MBSTRING_SHARED_LIBADD)

  for dir in $PHP_MBSTRING_EXTRA_BUILD_DIRS; do
    PHP_ADD_BUILD_DIR([$ext_builddir/$dir], 1)
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

  PHP_MBSTRING_ADD_INSTALL_HEADERS([mbstring.h])
  PHP_INSTALL_HEADERS([ext/mbstring], [$PHP_MBSTRING_INSTALL_HEADERS])
])

AC_DEFUN([PHP_MBSTRING_SETUP_MBREGEX], [
  if test "$PHP_MBREGEX" = "yes"; then
    PKG_CHECK_MODULES([ONIG], [oniguruma])
    PHP_EVAL_LIBLINE($ONIG_LIBS, MBSTRING_SHARED_LIBADD)
    PHP_EVAL_INCLINE($ONIG_CFLAGS)

    save_old_LDFLAGS=$LDFLAGS
    PHP_EVAL_LIBLINE([$MBSTRING_SHARED_LIBADD], LDFLAGS)
    AC_MSG_CHECKING([if oniguruma has an invalid entry for KOI8 encoding])
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <oniguruma.h>
    ]], [[
return (int)(ONIG_ENCODING_KOI8 + 1);
    ]])], [
      AC_MSG_RESULT([no])
    ], [
      AC_MSG_RESULT([yes])
      AC_DEFINE([PHP_ONIG_BAD_KOI8_ENTRY], [1], [define to 1 if oniguruma has an invalid entry for KOI8 encoding])
    ])
    LDFLAGS=$save_old_LDFLAGS

    PHP_MBSTRING_ADD_CFLAG([-DONIG_ESCAPE_UCHAR_COLLISION=1])
    PHP_MBSTRING_ADD_CFLAG([-DUChar=OnigUChar])

    AC_DEFINE([HAVE_MBREGEX], 1, [whether to have multibyte regex support])

    PHP_MBSTRING_ADD_BASE_SOURCES([php_mbregex.c])
    PHP_MBSTRING_ADD_INSTALL_HEADERS([php_mbregex.h php_onig_compat.h])
  fi
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
    libmbfl/filters/mbfilter_ascii.c
    libmbfl/filters/mbfilter_base64.c
    libmbfl/filters/mbfilter_big5.c
    libmbfl/filters/mbfilter_byte2.c
    libmbfl/filters/mbfilter_byte4.c
    libmbfl/filters/mbfilter_cp1251.c
    libmbfl/filters/mbfilter_cp1252.c
    libmbfl/filters/mbfilter_cp1254.c
    libmbfl/filters/mbfilter_cp5022x.c
    libmbfl/filters/mbfilter_cp51932.c
    libmbfl/filters/mbfilter_cp850.c
    libmbfl/filters/mbfilter_cp866.c
    libmbfl/filters/mbfilter_cp932.c
    libmbfl/filters/mbfilter_cp936.c
    libmbfl/filters/mbfilter_gb18030.c
    libmbfl/filters/mbfilter_euc_cn.c
    libmbfl/filters/mbfilter_euc_jp.c
    libmbfl/filters/mbfilter_euc_jp_2004.c
    libmbfl/filters/mbfilter_euc_jp_win.c
    libmbfl/filters/mbfilter_euc_kr.c
    libmbfl/filters/mbfilter_euc_tw.c
    libmbfl/filters/mbfilter_htmlent.c
    libmbfl/filters/mbfilter_hz.c
    libmbfl/filters/mbfilter_iso2022_jp_ms.c
    libmbfl/filters/mbfilter_iso2022jp_2004.c
    libmbfl/filters/mbfilter_iso2022jp_mobile.c
    libmbfl/filters/mbfilter_iso2022_kr.c
    libmbfl/filters/mbfilter_iso8859_1.c
    libmbfl/filters/mbfilter_iso8859_10.c
    libmbfl/filters/mbfilter_iso8859_13.c
    libmbfl/filters/mbfilter_iso8859_14.c
    libmbfl/filters/mbfilter_iso8859_15.c
    libmbfl/filters/mbfilter_iso8859_16.c
    libmbfl/filters/mbfilter_iso8859_2.c
    libmbfl/filters/mbfilter_iso8859_3.c
    libmbfl/filters/mbfilter_iso8859_4.c
    libmbfl/filters/mbfilter_iso8859_5.c
    libmbfl/filters/mbfilter_iso8859_6.c
    libmbfl/filters/mbfilter_iso8859_7.c
    libmbfl/filters/mbfilter_iso8859_8.c
    libmbfl/filters/mbfilter_iso8859_9.c
    libmbfl/filters/mbfilter_jis.c
    libmbfl/filters/mbfilter_koi8r.c
    libmbfl/filters/mbfilter_armscii8.c
    libmbfl/filters/mbfilter_qprint.c
    libmbfl/filters/mbfilter_sjis.c
    libmbfl/filters/mbfilter_sjis_open.c
    libmbfl/filters/mbfilter_sjis_mobile.c
    libmbfl/filters/mbfilter_sjis_mac.c
    libmbfl/filters/mbfilter_sjis_2004.c
    libmbfl/filters/mbfilter_tl_jisx0201_jisx0208.c
    libmbfl/filters/mbfilter_ucs2.c
    libmbfl/filters/mbfilter_ucs4.c
    libmbfl/filters/mbfilter_uhc.c
    libmbfl/filters/mbfilter_utf16.c
    libmbfl/filters/mbfilter_utf32.c
    libmbfl/filters/mbfilter_utf7.c
    libmbfl/filters/mbfilter_utf7imap.c
    libmbfl/filters/mbfilter_utf8.c
    libmbfl/filters/mbfilter_utf8_mobile.c
    libmbfl/filters/mbfilter_uuencode.c
    libmbfl/filters/mbfilter_koi8u.c
    libmbfl/mbfl/mbfilter.c
    libmbfl/mbfl/mbfilter_8bit.c
    libmbfl/mbfl/mbfilter_pass.c
    libmbfl/mbfl/mbfilter_wchar.c
    libmbfl/mbfl/mbfl_convert.c
    libmbfl/mbfl/mbfl_encoding.c
    libmbfl/mbfl/mbfl_filter_output.c
    libmbfl/mbfl/mbfl_ident.c
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
  PHP_MBSTRING_ADD_INSTALL_HEADERS([libmbfl/config.h libmbfl/mbfl/eaw_table.h libmbfl/mbfl/mbfilter.h libmbfl/mbfl/mbfilter_8bit.h libmbfl/mbfl/mbfilter_pass.h libmbfl/mbfl/mbfilter_wchar.h libmbfl/mbfl/mbfl_consts.h libmbfl/mbfl/mbfl_convert.h libmbfl/mbfl/mbfl_defs.h libmbfl/mbfl/mbfl_encoding.h libmbfl/mbfl/mbfl_filter_output.h libmbfl/mbfl/mbfl_ident.h libmbfl/mbfl/mbfl_language.h libmbfl/mbfl/mbfl_memory_device.h libmbfl/mbfl/mbfl_string.h])
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
  AC_DEFINE([HAVE_MBSTRING],1,[whether to have multibyte string support])

  PHP_MBSTRING_ADD_BASE_SOURCES([mbstring.c php_unicode.c mb_gpc.c])

  if test "$PHP_MBREGEX" != "no"; then
    PHP_MBSTRING_SETUP_MBREGEX
  fi

  dnl libmbfl is required
  PHP_MBSTRING_SETUP_LIBMBFL
  PHP_MBSTRING_EXTENSION
fi
