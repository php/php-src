dnl $Id$
dnl config.m4 for extension pdo_odbc

PHP_ARG_WITH(pdo_odbc, for pdo_odbc support,
[  --with-pdo_odbc             Include pdo_odbc support])

if test "$PHP_PDO_ODBC" != "no"; then
  PHP_NEW_EXTENSION(pdo_odbc, pdo_odbc.c odbc_driver.c odbc_stmt.c, $ext_shared)
fi
