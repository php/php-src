dnl $Id$
dnl config.m4 for extension pcre

dnl By default we'll compile and link against the bundled PCRE library
dnl if DIR is supplied, we'll use that for linking

AC_MSG_CHECKING(whether to include PCRE support)
AC_ARG_WITH(pcre-regex,
[  --with-pcre-regex[=DIR] Include Perl Compatible Regular Expressions support],[
  if test "$withval" = "yes"; then
    EXTRA_LIBS="-Lext/pcre/pcrelib -lpcre $EXTRA_LIBS"
    PCRE_SUBDIR="pcrelib"
    AC_DEFINE(HAVE_PCRE, 1)
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(pcre)
  else
    AC_MSG_RESULT(no)
  fi
],[
  EXTRA_LIBS="-Lext/pcre/pcrelib -lpcre $EXTRA_LIBS"
  PCRE_SUBDIR="pcrelib"
  AC_DEFINE(HAVE_PCRE, 1)
  AC_MSG_RESULT(yes)
  PHP_EXTENSION(pcre)
]) 
AC_SUBST(PCRE_SUBDIR)
