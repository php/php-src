dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(mbstring, whether to enable multibyte string support,
[  --disable-mbstring      Disable multibyte string support], yes)

PHP_ARG_ENABLE(mbregex, whether to enable multibyte regex support,
[  --disable-mbregex         Disable multibyte regex support], yes)

PHP_ARG_ENABLE(mbstr_enc_trans, whether to enable encoding translation,
[  --enable-mbstr-enc-trans  Enable input encoding translation], no)

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
  PHP_NEW_EXTENSION(mbstring, mbfilter_ja.c mbfilter_cn.c mbfilter_tw.c mbfilter_kr.c mbfilter_ru.c mbfilter.c mbstring.c mbregex.c php_mbregex.c, $ext_shared)
else
  PHP_MBSTR_ENC_TRANS=no
fi

if test "$MBREGEX" != "no" ; then
  AC_DEFINE(HAVE_MBREGEX, 1, [whether to have multibyte regex support])
fi

if test "$PHP_MBSTR_ENC_TRANS" != "no" ; then
  AC_DEFINE(MBSTR_ENC_TRANS, 1, [whether to have encoding translation])
fi

