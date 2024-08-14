PHP_ARG_WITH([gettext],
  [for GNU gettext support],
  [AS_HELP_STRING([[--with-gettext[=DIR]]],
    [Include GNU gettext support])])

if test "$PHP_GETTEXT" != "no"; then
  for i in $PHP_GETTEXT /usr/local /usr; do
    AS_IF([test -r $i/include/libintl.h], [GETTEXT_DIR=$i; break;])
  done

  AS_VAR_IF([GETTEXT_DIR],,
    [AC_MSG_ERROR([Cannot locate header file libintl.h])])

  GETTEXT_LIBDIR=$GETTEXT_DIR/$PHP_LIBDIR
  GETTEXT_INCDIR=$GETTEXT_DIR/include

  O_LDFLAGS=$LDFLAGS
  LDFLAGS="$LDFLAGS -L$GETTEXT_LIBDIR"
  AC_CHECK_LIB([intl], [bindtextdomain], [
    GETTEXT_LIBS=intl
    GETTEXT_CHECK_IN_LIB=intl
    ],
    [AC_CHECK_LIB([c], [bindtextdomain], [
      GETTEXT_LIBS=
      GETTEXT_CHECK_IN_LIB=c
      ],
      [AC_MSG_FAILURE([Unable to find required intl library for gettext.])])])

  AC_DEFINE([HAVE_LIBINTL], [1], [Define to 1 if you have the 'intl' library.])
  PHP_NEW_EXTENSION([gettext], [gettext.c], [$ext_shared])
  PHP_SUBST([GETTEXT_SHARED_LIBADD])

  PHP_ADD_INCLUDE([$GETTEXT_INCDIR])

  AC_CHECK_LIB([$GETTEXT_CHECK_IN_LIB], [ngettext],
    [AC_DEFINE([HAVE_NGETTEXT], [1],
      [Define to 1 if you have the 'ngettext' function.])])
  AC_CHECK_LIB([$GETTEXT_CHECK_IN_LIB], [dngettext],
    [AC_DEFINE([HAVE_DNGETTEXT], [1],
      [Define to 1 if you have the 'dngettext' function.])])
  AC_CHECK_LIB([$GETTEXT_CHECK_IN_LIB], [dcngettext],
    [AC_DEFINE([HAVE_DCNGETTEXT], [1],
      [Define to 1 if you have the 'dcngettext' function.])])
  AC_CHECK_LIB([$GETTEXT_CHECK_IN_LIB], [bind_textdomain_codeset],
    [AC_DEFINE([HAVE_BIND_TEXTDOMAIN_CODESET], [1],
      [Define to 1 if you have the 'bind_textdomain_codeset' function.])])
  LDFLAGS=$O_LDFLAGS

  AS_VAR_IF([GETTEXT_LIBS],,,
    [PHP_ADD_LIBRARY_WITH_PATH([$GETTEXT_LIBS],
      [$GETTEXT_LIBDIR],
      [GETTEXT_SHARED_LIBADD])])
fi
