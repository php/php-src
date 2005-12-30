dnl
dnl $Id$
dnl

AC_DEFUN([PHP_MBSTRING_ADD_SOURCES], [
  PHP_MBSTRING_SOURCES="$PHP_MBSTRING_SOURCES $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_BUILD_DIR], [
  PHP_MBSTRING_EXTRA_BUILD_DIRS="$PHP_MBSTRING_EXTRA_BUILD_DIRS $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_INCLUDE], [
  PHP_MBSTRING_EXTRA_INCLUDES="$PHP_MBSTRING_EXTRA_INCLUDES $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_CONFIG_HEADER], [
  PHP_MBSTRING_EXTRA_CONFIG_HEADERS="$PHP_MBSTRING_EXTRA_CONFIG_HEADERS $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_CFLAG], [
  PHP_MBSTRING_CFLAGS="$PHP_MBSTRING_CFLAGS $1"
])

AC_DEFUN([PHP_MBSTRING_EXTENSION], [
  PHP_NEW_EXTENSION(mbstring, $PHP_MBSTRING_SOURCES, $ext_shared,, $PHP_MBSTRING_CFLAGS)
  PHP_SUBST(MBSTRING_SHARED_LIBADD)

  for dir in $PHP_MBSTRING_EXTRA_INCLUDES; do
    PHP_ADD_INCLUDE([$ext_srcdir/$dir])
  done

  for dir in $PHP_MBSTRING_EXTRA_BUILD_DIRS; do
    PHP_ADD_BUILD_DIR([$ext_builddir/$dir])
  done
  
  if test "$ext_shared" = "no"; then
    out="php_config.h"
  else
    if test -f "$ext_builddir/config.h.in"; then
      out="$abs_builddir/config.h"
    else
      out="php_config.h"
    fi
  fi
  
  for cfg in $PHP_MBSTRING_EXTRA_CONFIG_HEADERS; do
    cat > $ext_srcdir/$cfg <<EOF
#include "$out"
EOF
  done

])

AC_DEFUN([PHP_MBSTRING_SETUP_MBREGEX], [
  if test "$PHP_MBREGEX" = "yes"; then
    AC_DEFINE([HAVE_MBREGEX], 1, [whether to have multibyte regex support])

    PHP_MBSTRING_ADD_INCLUDE([mbregex])
    PHP_MBSTRING_ADD_BUILD_DIR([mbregex])

    PHP_MBSTRING_ADD_SOURCES([
      php_mbregex.c
      mbregex/mbregex.c
    ])
  fi
])

AC_DEFUN([PHP_MBSTRING_SETUP_LIBMBFL], [
  dnl libmbfl is required and can not be disabled
  if test "$PHP_LIBMBFL" = "yes" || test "$PHP_LIBMBFL" = "no"; then
    dnl
    dnl Bundled libmbfl
    dnl
    PHP_MBSTRING_ADD_BUILD_DIR([libmbfl])
    PHP_MBSTRING_ADD_BUILD_DIR([libmbfl/mbfl])
    PHP_MBSTRING_ADD_BUILD_DIR([libmbfl/filters])
    PHP_MBSTRING_ADD_BUILD_DIR([libmbfl/nls])
    PHP_MBSTRING_ADD_INCLUDE([libmbfl])
    PHP_MBSTRING_ADD_INCLUDE([libmbfl/mbfl])
    PHP_MBSTRING_ADD_CONFIG_HEADER([libmbfl/config.h])

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
     libmbfl/filters/mbfilter_cp866.c
     libmbfl/filters/mbfilter_cp932.c
     libmbfl/filters/mbfilter_cp936.c
     libmbfl/filters/mbfilter_euc_cn.c
     libmbfl/filters/mbfilter_euc_jp.c
     libmbfl/filters/mbfilter_euc_jp_win.c
     libmbfl/filters/mbfilter_euc_kr.c
     libmbfl/filters/mbfilter_euc_tw.c
     libmbfl/filters/mbfilter_htmlent.c
     libmbfl/filters/mbfilter_hz.c
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
     libmbfl/filters/mbfilter_qprint.c
     libmbfl/filters/mbfilter_armscii8.c
     libmbfl/filters/mbfilter_sjis.c
     libmbfl/filters/mbfilter_ucs2.c
     libmbfl/filters/mbfilter_ucs4.c
     libmbfl/filters/mbfilter_uhc.c
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
     libmbfl/mbfl/mbfl_ident.c
     libmbfl/mbfl/mbfl_language.c
     libmbfl/mbfl/mbfl_memory_device.c
     libmbfl/mbfl/mbfl_string.c
     libmbfl/mbfl/mbfl_allocators.c
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
    ])
    PHP_MBSTRING_ADD_CFLAG([-DHAVE_CONFIG_H])
  else
    dnl
    dnl External libmfl
    dnl  
    for inc in include include/mbfl-1.0 include/mbfl; do
      if test -f "$PHP_LIBMBFL/$inc/mbfilter.h"; then
        PHP_LIBMBFL_INCLUDE="$inc"
        break
      fi
    done

    if test -z "$PHP_LIBMBFL_INCLUDE"; then
      AC_MSG_ERROR([mbfilter.h not found. Please reinstall libmbfl library.])
    else 
      PHP_ADD_INCLUDE([$PHP_LIBMBFL_INCLUDE])
    fi

    PHP_CHECK_LIBRARY(mbfl, mbfl_buffer_converter_new, [
      PHP_ADD_LIBRARY_WITH_PATH(mbfl, $PHP_LIBMBFL/lib, MBSTRING_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([Problem with libmbfl. Please check config.log for more information.])
    ], [
      -LPHP_LIBMBFL/lib
    ])
  fi
])

dnl
dnl Main config
dnl

PHP_ARG_ENABLE(mbstring, whether to enable multibyte string support,
[  --enable-mbstring       Enable multibyte string support.])

PHP_ARG_ENABLE([mbregex], [whether to enable multibyte regex support],
[  --disable-mbregex         MBSTRING: Disable multibyte regex support], yes, no)

PHP_ARG_WITH(libmbfl, [for external libmbfl],
[  --with-libmbfl[=DIR]      MBSTRING: Use external libmbfl. DIR is the libmbfl install prefix.
                            If DIR is not set, the bundled libmbfl will be used.], no, no)

if test "$PHP_MBSTRING" != "no"; then  
  AC_DEFINE(HAVE_MBSTRING,1,[whether to have multibyte string support])

  PHP_MBSTRING_ADD_SOURCES([mbstring.c php_unicode.c])

  if test "$PHP_MBREGEX" != "no"; then
    PHP_MBSTRING_SETUP_MBREGEX
  fi

  dnl libmbfl is required
  PHP_MBSTRING_SETUP_LIBMBFL
  PHP_MBSTRING_EXTENSION
fi


