dnl $Id$
dnl config.m4 for extension pdo

AC_ARG_WITH(broken-pear-install,
[],[
  if test "x$withval" = "xyes"; then
  AC_MSG_ERROR([
Due to a bug in the pear installer you should install the PDO package manually
using the following steps:

 1. Download the PDO package to your local machine:
 
    % wget http://pecl.php.net/get/PDO

 2. Determine your PHP bin dir.
    If your php5 cli binary lives at /usr/local/php5/bin/php
    the bin dir is /usr/local/php5/bin

 3. Set your path so that your PHP bin dir is at the front:
    export PATH="/usr/local/php5/bin:\$PATH"

 4. Manually build:
 
    % tar xzf PDO-0.2.tgz
    % cd PDO-0.2
    % phpize
    % ./configure
    % make
    % sudo -s
    # make install
    # echo extension=pdo.so >> /usr/local/php5/lib/php.ini

We apologize for the inconvenience, and hope to resolve this problem
in a future PHP/PEAR release.

])
fi
])

PHP_ARG_ENABLE(pdo, whether to disable PDO support,
[  --disable-pdo            Disable PHP Data Objects support], yes)

if test "$PHP_PDO" != "no"; then
  PHP_NEW_EXTENSION(pdo, pdo.c pdo_dbh.c pdo_stmt.c pdo_sql_parser.c pdo_sqlstate.c, $ext_shared)
  PHP_ADD_MAKEFILE_FRAGMENT
fi
