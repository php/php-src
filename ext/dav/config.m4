dnl $Id$
dnl config.m4 for extension dav
dnl don't forget to call PHP_EXTENSION(dav)

AC_MSG_CHECKING(whether to enable DAV support through mod_dav)
AC_ARG_WITH(mod-dav,
[  --with-mod-dav=DIR      Include DAV support through Apache's mod_dav,
                          DIR is mod_dav's installation directory (Apache
                          module version only!)],
[
  if test "$withval" = "yes"; then
    AC_MSG_ERROR(Must give parameter to --with-mod-dav!)
  else
    if test "$withval" != "no"; then
      AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE_MOD_DAV, 1)
      CFLAGS="$CFLAGS -DHAVE_MOD_DAV -I$withval"
      INCLUDES="$INCLUDES -I$withval"
      PHP_EXTENSION(dav)
    else
      AC_MSG_RESULT(no)
      AC_DEFINE(HAVE_MOD_DAV, 0)
    fi
  fi
],[
  AC_MSG_RESULT(no)
  AC_DEFINE(HAVE_MOD_DAV, 0)
])

