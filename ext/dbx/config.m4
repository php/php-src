dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(dbx,whether to enable dbx support,
[  --enable-dbx            Enable dbx])

if test "$PHP_DBX" != "no"; then
  PHP_NEW_EXTENSION(dbx, dbx.c dbx_mysql.c dbx_odbc.c dbx_pgsql.c dbx_mssql.c dbx_fbsql.c dbx_oci8.c dbx_sybasect.c, $ext_shared)
fi
