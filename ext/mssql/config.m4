dnl
dnl $Id$
dnl

PHP_ARG_WITH(mssql,for MSSQL support via FreeTDS,
[  --with-mssql[=DIR]      Include MSSQL-DB support.  DIR is the FreeTDS home
                          directory, defaults to /usr/local/freetds.])


if test "$PHP_MSSQL" != "no"; then

  FREETDS_INSTALLATION_DIR=""
  if test "$PHP_MSSQL" = "yes"; then

    for i in /usr/local /usr; do
      if test -f $i/include/tds.h; then
        FREETDS_INSTALLATION_DIR=$i
        break
      fi
    done

    if test -z "$FREETDS_INSTALLATION_DIR"; then
      AC_MSG_ERROR(Cannot find FreeTDS in known installation directories)
    fi

  elif test "$PHP_MSSQL" != "no"; then

    if test -f $PHP_MSSQL/include/tds.h; then
      FREETDS_INSTALLATION_DIR=$PHP_MSSQL
    else
      AC_MSG_ERROR(Directory $PHP_MSSQL is not a FreeTDS installation directory)
    fi
  fi  

  if test ! -r "$FREETDS_INSTALLATION_DIR/lib/libtds.a"; then
     AC_MSG_ERROR(Could not find $FREETDS_INSTALLATION_DIR/lib/libtds.a)
  fi

  MSSQL_INCDIR=$FREETDS_INSTALLATION_DIR/include
  MSSQL_LIBDIR=$FREETDS_INSTALLATION_DIR/lib

  PHP_ADD_INCLUDE($MSSQL_INCDIR)
  PHP_ADD_LIBRARY_WITH_PATH(sybdb, $MSSQL_LIBDIR, MSSQL_SHARED_LIBADD)
  PHP_NEW_EXTENSION(mssql, php_mssql.c, $ext_shared)
  AC_CHECK_LIB(dnet_stub, dnet_addr,
     [ PHP_ADD_LIBRARY_WITH_PATH(dnet_stub,,MSSQL_SHARED_LIBADD)
        AC_DEFINE(HAVE_LIBDNET_STUB,1,[ ])
     ])
  AC_DEFINE(HAVE_MSSQL,1,[ ])
  AC_DEFINE(HAVE_FREETDS,1,[ ])
  PHP_SUBST(MSSQL_SHARED_LIBADD)
fi
