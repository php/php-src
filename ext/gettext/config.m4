dnl $Id$
dnl config.m4 for extension gettext
dnl don't forget to call PHP_EXTENSION(gettext)

AC_MSG_CHECKING(whether to include GNU gettext support)
AC_ARG_WITH(gettext,
[  --with-gettext[=DIR]    Include GNU gettext support.  DIR is the gettext
                          install directory, defaults to /usr/local],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
        GETTEXT_INCDIR=/usr/local/include
        test -f /usr/local/include/libintl.h && GETTEXT_INCDIR=/usr/local/include/
        GETTEXT_LIBDIR=/usr/local/lib
    else
        GETTEXT_INCDIR=$withval/include
        test -f $withval/include/libintl.h && GETTEXT_INCDIR=$withval/include
        GETTEXT_LIBDIR=$withval/lib
    fi
    GETTEXT_INCLUDE=-I$GETTEXT_INCDIR
    GETTEXT_LFLAGS=-L$GETTEXT_LIBDIR
    O_CPPFLAGS="$CPPFLAGS"
    O_LDFLAGS=$LDFLAGS
    CPPFLAGS="$CPPFLAGS -I$GETTEXT_INCDIR"
    LDFLAGS="$LDFLAGS -L$GETTEXT_LIBDIR"
    GETTEXT_LIBS=
    AC_MSG_RESULT(yes)
    AC_CHECK_LIB(intl, bindtextdomain, GETTEXT_LIBS="-lintl",[
        AC_CHECK_LIB(c, bindtextdomain, GETTEXT_LIBS=,[
            AC_MSG_ERROR(Unable to find required gettext library)
        ],)
    ],)

    AC_DEFINE(HAVE_LIBINTL,1,[ ])
    PHP_EXTENSION(gettext)

	EXTRA_LIBS="$EXTRA_LIBS $GETTEXT_LFLAGS $GETTEXT_LIBS"
	INCLUDES="$INCLUDES $GETTEXT_INCLUDE"

  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
