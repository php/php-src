dnl
dnl $Id$
dnl

PHP_ARG_WITH(mssql,for MSSQL support via FreeTDS,
[  --with-mssql[=DIR]      Include MSSQL-DB support.  DIR is the FreeTDS home
                          directory, defaults to /usr/local/freetds.])


if test "$PHP_MSSQL" != "no"; then
  if test "$PHP_MSSQL" = "yes"; then
    MSSQL_INCDIR=/usr/local/freetds/include
    MSSQL_LIBDIR=/usr/local/freetds/lib
  else
    MSSQL_INCDIR=$PHP_MSSQL/include
    MSSQL_LIBDIR=$PHP_MSSQL/lib
  fi
  PHP_ADD_INCLUDE($MSSQL_INCDIR)
  PHP_ADD_LIBRARY_WITH_PATH(sybdb, $MSSQL_LIBDIR, MSSQL_SHARED_LIBADD)
  PHP_NEW_EXTENSION(mssql, php_mssql.c, $ext_shared)
  AC_CHECK_LIB(dnet_stub, dnet_addr,
     [ PHP_ADD_LIBRARY_WITH_PATH(dnet_stub,,MSSQL_SHARED_LIBADD)
        AC_DEFINE(HAVE_LIBDNET_STUB,1,[ ])
     ])
  AC_DEFINE(HAVE_MSSQL,1,[ ])
  AC_DEFINE(HAVE_FREETDS,1,[ ])
fi
