dnl $Id$
dnl config.m4 for extension jstring

PHP_ARG_ENABLE(jstring, whether to enable japanese string support,
[  --enable-jstring        Enable japanese string support])

if test "$PHP_JSTRING" != "no"; then
  AC_DEFINE(HAVE_JSTRING,1,[ ])
  PHP_EXTENSION(jstring, $ext_shared)
fi

AC_MSG_CHECKING(whether to enable japanese encoding translation)
AC_ARG_ENABLE(jstr_enc_trans,
[  --enable-jstr-enc-trans   Enable japanese encoding translation],[
  if test "$enableval" = "yes" ; then
    AC_DEFINE(JSTR_ENC_TRANS, 1, [ ])
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
