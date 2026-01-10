PHP_ARG_ENABLE([pdo],
  [whether to enable PDO support],
  [AS_HELP_STRING([--disable-pdo],
    [Disable PHP Data Objects support])],
  [yes])

if test "$PHP_PDO" != "no"; then
  dnl Make sure $PHP_PDO is 'yes' when it's not 'no' :)
  PHP_PDO=yes

  PHP_NEW_EXTENSION([pdo],
    [pdo.c pdo_dbh.c pdo_stmt.c pdo_sql_parser.c pdo_sqlstate.c],
    [$ext_shared])
  PHP_ADD_EXTENSION_DEP(pdo, spl)
  PHP_INSTALL_HEADERS([ext/pdo], m4_normalize([
    pdo_sql_parser.h
    php_pdo_driver.h
    php_pdo_error.h
    php_pdo.h
  ]))
  PHP_ADD_MAKEFILE_FRAGMENT
fi
