dnl $Id$
dnl config.m4 for extension pcre

dnl By default we'll compile and link against the bundled PCRE library
dnl if DIR is supplied, we'll use that for linking

AC_MSG_CHECKING(whether to use PCRE library)
AC_ARG_WITH(pcre-regex,
[  --with-pcre-regex[=DIR] Enable Perl Compatible Regular Expressions support],[
  case "$withval" in
    no)
    	AC_MSG_RESULT(no) ;;
    yes)
        EXTRA_LIBS="-Lext/pcre/pcrelib -lpcre $EXTRA_LIBS"
        PCRE_SUBDIR="pcrelib"
    	AC_DEFINE(HAVE_PCRE, 1)
        AC_DEFINE(HAVE_BUNDLED_PCRE, 1)
    	AC_MSG_RESULT(yes)
        PHP_EXTENSION(pcre) ;;
    *)
        test -f $withval/pcre.h && PCRE_INCLUDE="-I$withval"
        test -f $withval/libpcre.a && PCRE_LIB="-L$withval -lpcre"
	
        if test -n "$PCRE_INCLUDE" && test -n "$PCRE_LIB" ; then
          INCLUDES="$PCRE_INCLUDE $INCLUDES"
          EXTRA_LIBS="$PCRE_LIB $EXTRA_LIBS"
          AC_DEFINE(HAVE_PCRE, 1)
          AC_DEFINE(HAVE_BUNDLED_PCRE, 0)
          PCRE_SUBDIR=
          AC_MSG_RESULT(yes)
          PHP_EXTENSION(pcre)
        else
          AC_MSG_ERROR(Unable to find pcre.h and libpcre.a in $withval)
        fi ;;
  esac
],[
  EXTRA_LIBS="-Lext/pcre/pcrelib -lpcre $EXTRA_LIBS"
  PCRE_SUBDIR="pcrelib"
  AC_DEFINE(HAVE_PCRE, 1)
  AC_DEFINE(HAVE_BUNDLED_PCRE, 1)
  AC_MSG_RESULT(yes)
  PHP_EXTENSION(pcre)
]) 
AC_SUBST(PCRE_SUBDIR)
