dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(mbstring, whether to enable multibyte string support,
[  --enable-mbstring       Enable multibyte string support])

if test "$PHP_MBSTRING" != "no"; then  
  AC_DEFINE(HAVE_MBSTRING,1,[whether to have multibyte string support])

  if test "$PHP_MBSTRING" != "no" -o "$PHP_MBSTRING" = "ja"; then
    AC_DEFINE(HAVE_MBSTR_JA,1,[whether to have japanese support])
  fi
  if test "$PHP_MBSTRING" = "cn"; then
    AC_DEFINE(HAVE_MBSTR_CN,1,[whether to have simplified chinese support])
  fi
  if test "$PHP_MBSTRING" = "tw"; then
    AC_DEFINE(HAVE_MBSTR_TW,1,[whether to have traditional chinese support])
  fi
  if test "$PHP_MBSTRING" = "kr"; then
    AC_DEFINE(HAVE_MBSTR_KR,1,[whether to have korean support])
  fi
  if test "$PHP_MBSTRING" = "ru"; then
    AC_DEFINE(HAVE_MBSTR_RU,1,[whether to have russian support])
  fi
  if test "$PHP_MBSTRING" = "all"; then
    AC_DEFINE(HAVE_MBSTR_JA,1,[whether to have japanese support])
    AC_DEFINE(HAVE_MBSTR_CN,1,[whether to have simplified chinese support])
    AC_DEFINE(HAVE_MBSTR_TW,1,[whether to have traditional chinese support])
    AC_DEFINE(HAVE_MBSTR_KR,1,[whether to have korean support])
    AC_DEFINE(HAVE_MBSTR_RU,1,[whether to have russian support])
  fi

  PHP_NEW_EXTENSION(mbstring, mbfilter_ja.c mbfilter_cn.c mbfilter_tw.c mbfilter_kr.c mbfilter_ru.c mbfilter.c mbstring.c mbregex.c php_mbregex.c html_entities.c php_unicode.c mb_gpc.c, $ext_shared)
fi


PHP_ARG_ENABLE(mbregex, whether to enable multibyte regex support,
[  --enable-mbregex        Enable multibyte regex support], no, no)

if test "$PHP_MBREGEX" != "no" ; then
  AC_DEFINE(HAVE_MBREGEX, 1, [whether to have multibyte regex support])
fi
