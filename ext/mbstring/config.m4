dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(mbstring, whether to enable multibyte string support,
[  --disable-mbstring       Disable multibyte string support], yes)

PHP_ARG_ENABLE(mbstr_enc_trans, whether to enable encoding translation,
[  --enable-mbstr-enc-trans Enable encoding translation], yes)

PHP_ARG_ENABLE(mbregex, whether to enable multibyte regex support,
[  --enable-mbregex         Enable multibyte regex support], yes)

if test "$PHP_MBSTRING" != "no"; then
  AC_DEFINE(HAVE_MBSTRING,1,[whether to have multibyte string support])
  PHP_NEW_EXTENSION(mbstring, mbfilter_ja.c mbfilter.c mbstring.c mbregex.c php_mbregex.c, $ext_shared)
fi

if test "$PHP_MBSTR_ENC_TRANS" != "no" ; then
  AC_DEFINE(MBSTR_ENC_TRANS, 1, [whether to have encoding translation])
fi

if test "$MBREGEX" != "no" ; then
  AC_DEFINE(HAVE_MBREGEX, 1, [whether to have multibyte regex support])
fi
