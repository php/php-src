dnl
dnl $Id$
dnl

PHP_ARG_WITH(ovrimos, for Ovrimos SQL Server support,
[  --with-ovrimos[=DIR]    Include Ovrimos SQL Server support. DIR is the
                          Ovrimos libsqlcli install directory])

if test "$PHP_OVRIMOS" != "no"; then
  for i in $PHP_OVRIMOS /usr/local /usr; do
    test -f $i/include/sqlcli.h && OVRIMOS_DIR=$i && break
  done

  if test -z "$OVRIMOS_DIR"; then
    AC_MSG_ERROR(Please reinstall Ovrimos' libsqlcli - I cannot find sqlcli.h)
  fi

  PHP_ADD_INCLUDE($OVRIMOS_DIR/include)
  LDFLAGS="$LDFLAGS $ld_runpath_switch$OVRIMOS_DIR/lib -L$OVRIMOS_DIR/lib"
  AC_CHECK_LIB(sqlcli, main)
  PHP_ADD_LIBRARY_WITH_PATH(sqlcli, $OVRIMOS_DIR/lib, OVRIMOS_SHARED_LIBADD)

  PHP_NEW_EXTENSION(ovrimos, ovrimos.c, $ext_shared)
  PHP_SUBST(OVRIMOS_SHARED_LIBADD)
  AC_DEFINE(HAVE_LIBSQLCLI,1,[ ])
fi
