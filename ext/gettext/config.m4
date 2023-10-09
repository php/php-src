PHP_ARG_WITH([gettext],
  [for GNU gettext support],
  [AS_HELP_STRING([[--with-gettext[=DIR]]],
    [Include GNU gettext support])])

if test "$PHP_GETTEXT" != "no"; then
  for i in $PHP_GETTEXT /usr/local /usr; do
    test -r $i/include/libintl.h && GETTEXT_DIR=$i && break
  done

  if test -z "$GETTEXT_DIR"; then
    AC_MSG_ERROR(Cannot locate header file libintl.h)
  fi

  GETTEXT_LIBDIR=$GETTEXT_DIR/$PHP_LIBDIR
  GETTEXT_INCDIR=$GETTEXT_DIR/include

  O_LDFLAGS=$LDFLAGS
  LDFLAGS="$LDFLAGS -L$GETTEXT_LIBDIR"
  AC_CHECK_LIB(intl, bindtextdomain, [
    GETTEXT_LIBS=intl
    GETTEXT_CHECK_IN_LIB=intl
    ],
    [
    AC_CHECK_LIB(c, bindtextdomain, [
      GETTEXT_LIBS=
      GETTEXT_CHECK_IN_LIB=c
    ],[
      AC_MSG_ERROR(Unable to find required gettext library)
    ])
  ]
  )

  AC_DEFINE(HAVE_LIBINTL,1,[ ])
  PHP_NEW_EXTENSION(gettext, gettext.c, $ext_shared)
  PHP_SUBST(GETTEXT_SHARED_LIBADD)

  PHP_ADD_INCLUDE($GETTEXT_INCDIR)

  AC_CHECK_LIB($GETTEXT_CHECK_IN_LIB, ngettext,  [AC_DEFINE(HAVE_NGETTEXT, 1, [ ])])
  AC_CHECK_LIB($GETTEXT_CHECK_IN_LIB, dngettext,  [AC_DEFINE(HAVE_DNGETTEXT, 1, [ ])])
  AC_CHECK_LIB($GETTEXT_CHECK_IN_LIB, dcngettext,  [AC_DEFINE(HAVE_DCNGETTEXT, 1, [ ])])
  AC_CHECK_LIB($GETTEXT_CHECK_IN_LIB, bind_textdomain_codeset,  [AC_DEFINE(HAVE_BIND_TEXTDOMAIN_CODESET, 1, [ ])])
  LDFLAGS=$O_LDFLAGS

  if test -n "$GETTEXT_LIBS"; then
    PHP_ADD_LIBRARY_WITH_PATH($GETTEXT_LIBS, $GETTEXT_LIBDIR, GETTEXT_SHARED_LIBADD)
  fi
fi
