dnl
dnl $Id$
dnl

PHP_ARG_WITH(pdo-firebird,for Firebird support for PDO,
[  --with-pdo-firebird[=DIR]  Include Firebird support for PDO.  DIR is the Firebird base
                          install directory, defaults to /opt/firebird])

if test "$PHP_PDO_FIREBIRD" != "no"; then
  if test "$PHP_PDO_FIREBIRD" = "yes"; then
    FIREBIRD_INCDIR=/opt/firebird/include
    FIREBIRD_LIBDIR=/opt/firebird/lib
  else
    FIREBIRD_INCDIR=$PHP_PDO_FIREBIRD/include
    FIREBIRD_LIBDIR=$PHP_PDO_FIREBIRD/lib
  fi

  PHP_CHECK_LIBRARY(gds, isc_detach_database,
  [
    FIREBIRD_LIBNAME=gds
  ], [
    PHP_CHECK_LIBRARY(ib_util, isc_detach_database,
    [
      FIREBIRD_LIBNAME=ib_util
    ], [
      AC_MSG_ERROR([libgds or libib_util not found! Check config.log for more information.])
    ], [
      -L$FIREBIRD_LIBDIR
    ])
  ], [
    -L$FIREBIRD_LIBDIR
  ])
  
  if test -f $prefix/include/php/ext/pdo/php_pdo_driver.h; then
    pdo_inc_path=$prefix/include/php/ext
  elif test -f $abs_srcdir/include/php/ext/pdo/php_pdo_driver.h; then
    pdo_inc_path=$abs_srcdir/ext
  elif test -f ext/pdo/php_pdo_driver.h; then
    pdo_inc_path=ext
  else
   AC_MSG_ERROR([Cannot find php_pdo_driver.h.])
  fi
				  
  PHP_ADD_LIBRARY_WITH_PATH($FIREBIRD_LIBNAME, $FIREBIRD_LIBDIR, PDO_FIREBIRD_SHARED_LIBADD)
  PHP_ADD_INCLUDE($FIREBIRD_INCDIR)
  AC_DEFINE(HAVE_PDO_FIREBIRD,1,[ ])
  PHP_NEW_EXTENSION(pdo_firebird, pdo_firebird.c firebird_driver.c firebird_statement.c, $ext_shared,,-I$pdo_inc_path)
  PHP_SUBST(PDO_FIREBIRD_SHARED_LIBADD)
fi
