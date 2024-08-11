PHP_ARG_WITH([pdo-firebird],
  [for Firebird support for PDO],
  [AS_HELP_STRING([[--with-pdo-firebird[=DIR]]],
    [PDO: Firebird support. DIR is the Firebird base install directory
    [/opt/firebird]])])

if test "$PHP_PDO_FIREBIRD" != "no"; then
  AC_PATH_PROG([FB_CONFIG], [fb_config], [no])

  if test -x "$FB_CONFIG" && test "$PHP_PDO_FIREBIRD" = "yes"; then
    AC_MSG_CHECKING([for libfbconfig])
    FB_CFLAGS=`$FB_CONFIG --cflags`
    FB_LIBDIR=`$FB_CONFIG --libs`
    FB_VERSION=`$FB_CONFIG --version`
    AC_MSG_RESULT([version $FB_VERSION])
    PHP_EVAL_LIBLINE([$FB_LIBDIR], [PDO_FIREBIRD_SHARED_LIBADD])
    PHP_EVAL_INCLINE([$FB_CFLAGS])

  else
    if test "$PHP_PDO_FIREBIRD" = "yes"; then
      FIREBIRD_INCDIR=
      FIREBIRD_LIBDIR=
      FIREBIRD_LIBDIR_FLAG=
    else
      FIREBIRD_INCDIR=$PHP_PDO_FIREBIRD/include
      FIREBIRD_LIBDIR=$PHP_PDO_FIREBIRD/$PHP_LIBDIR
      FIREBIRD_LIBDIR_FLAG=-L$FIREBIRD_LIBDIR
    fi

    PHP_CHECK_LIBRARY([fbclient], [isc_detach_database],
      [FIREBIRD_LIBNAME=fbclient],
      [PHP_CHECK_LIBRARY([gds], [isc_detach_database],
        [FIREBIRD_LIBNAME=gds],
        [PHP_CHECK_LIBRARY([ib_util], [isc_detach_database],
          [FIREBIRD_LIBNAME=ib_util],
          [AC_MSG_FAILURE([libfbclient, libgds or libib_util not found.])],
          [$FIREBIRD_LIBDIR_FLAG])],
        [$FIREBIRD_LIBDIR_FLAG])],
      [$FIREBIRD_LIBDIR_FLAG])
    PHP_ADD_LIBRARY_WITH_PATH([$FIREBIRD_LIBNAME],
      [$FIREBIRD_LIBDIR],
      [PDO_FIREBIRD_SHARED_LIBADD])
    PHP_ADD_INCLUDE([$FIREBIRD_INCDIR])
  fi

  PHP_CHECK_PDO_INCLUDES

  PHP_NEW_EXTENSION([pdo_firebird],
    [pdo_firebird.c firebird_driver.c firebird_statement.c],
    [$ext_shared])
  PHP_SUBST([PDO_FIREBIRD_SHARED_LIBADD])
  PHP_ADD_EXTENSION_DEP(pdo_firebird, pdo)
fi
