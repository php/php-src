dnl $Id$
dnl config.m4 for extension pcre

AC_MSG_CHECKING(for PCRE support)
AC_ARG_WITH(pcre,
[  --with-pcre             Include Perl Compatible Regular Expressions support],[
  if test "$withval" = "yes"; then
    PCRE_LIBS="-lpcre"
    AC_DEFINE(HAVE_LIBPCRE, 1)
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(pcre)
    EXTRA_LIBS="$EXTRA_LIBS $PCRE_LIBS"
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
]) 
