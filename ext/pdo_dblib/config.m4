dnl
dnl $Id$
dnl

PHP_ARG_WITH(pdo-dblib, for PDO_DBLIB support via FreeTDS,
[  --with-pdo-dblib[=DIR]    PDO: DBLIB-DB support.  DIR is the FreeTDS home directory])

if test "$PHP_PDO_DBLIB" != "no"; then

  if test "$PHP_PDO" = "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([PDO is not enabled! Add --enable-pdo to your configure line.])
  fi

  PDO_FREETDS_INSTALLATION_DIR=""
  if test "$PHP_PDO_DBLIB" = "yes"; then

    for i in /usr/local /usr; do
      if test -f $i/include/tds.h; then
        PDO_FREETDS_INSTALLATION_DIR=$i
        break
      fi
    done

    if test -z "$PDO_FREETDS_INSTALLATION_DIR"; then
      AC_MSG_ERROR(Cannot find FreeTDS in known installation directories)
    fi

  elif test "$PHP_PDO_DBLIB" != "no"; then

    if test -f $PHP_PDO_DBLIB/include/tds.h; then
      PDO_FREETDS_INSTALLATION_DIR=$PHP_PDO_DBLIB
    else
      AC_MSG_ERROR(Directory $PHP_PDO_DBLIB is not a FreeTDS installation directory)
    fi
  fi  

  if test "x$PHP_LIBDIR" = "x" ; then
    PHP_LIBDIR=lib
  fi

  if test ! -r "$PDO_FREETDS_INSTALLATION_DIR/$PHP_LIBDIR/libtds.a" && test ! -r "$PDO_FREETDS_INSTALLATION_DIR/$PHP_LIBDIR/libtds.so"; then
     AC_MSG_ERROR(Could not find $PDO_FREETDS_INSTALLATION_DIR/$PHP_LIBDIR/libtds.[a|so])
  fi

  PDO_DBLIB_INCDIR=$PDO_FREETDS_INSTALLATION_DIR/include
  PDO_DBLIB_LIBDIR=$PDO_FREETDS_INSTALLATION_DIR/$PHP_LIBDIR

  PHP_ADD_INCLUDE($PDO_DBLIB_INCDIR)
  PHP_ADD_LIBRARY_WITH_PATH(sybdb, $PDO_DBLIB_LIBDIR, PDO_DBLIB_SHARED_LIBADD)
  ifdef([PHP_CHECK_PDO_INCLUDES],
  [
    PHP_CHECK_PDO_INCLUDES
  ],[
    AC_MSG_CHECKING([for PDO includes])
    if test -f $abs_srcdir/include/php/ext/pdo/php_pdo_driver.h; then
      pdo_inc_path=$abs_srcdir/ext
    elif test -f $abs_srcdir/ext/pdo/php_pdo_driver.h; then
      pdo_inc_path=$abs_srcdir/ext
    elif test -f $prefix/include/php/ext/pdo/php_pdo_driver.h; then
      pdo_inc_path=$prefix/include/php/ext
    else
      AC_MSG_ERROR([Cannot find php_pdo_driver.h.])
    fi
    AC_MSG_RESULT($pdo_inc_path)
  ])

  PDO_DBLIB_DEFS="-DPDO_DBLIB_FLAVOUR=\\\"freetds\\\""
  PHP_NEW_EXTENSION(pdo_dblib, pdo_dblib.c dblib_driver.c dblib_stmt.c, $ext_shared,,-I$pdo_inc_path $PDO_DBLIB_DEFS)
  AC_CHECK_LIB(dnet_stub, dnet_addr,
     [ PHP_ADD_LIBRARY_WITH_PATH(dnet_stub,,PDO_DBLIB_SHARED_LIBADD)
        AC_DEFINE(HAVE_LIBDNET_STUB,1,[ ])
     ])
  AC_DEFINE(HAVE_PDO_DBLIB,1,[ ])
  AC_DEFINE(HAVE_FREETDS,1,[ ])
  PHP_SUBST(PDO_DBLIB_SHARED_LIBADD)

  ifdef([PHP_ADD_EXTENSION_DEP],
  [
    PHP_ADD_EXTENSION_DEP(pdo_dblib, pdo)
  ])
fi
