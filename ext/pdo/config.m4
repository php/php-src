dnl $Id$
dnl config.m4 for extension pdo
dnl vim:se ts=2 sw=2 et:

AC_ARG_WITH(broken-pear-install,
[],[
  if test "x$withval" = "xyes"; then
  case $host_alias in
  *darwin*)
    AC_MSG_ERROR([
Due to the way that loadable modules work on OSX/Darwin, you need to
compile the PDO package statically into the PHP core.

Please follow the instructions at: http://netevil.org/node.php?nid=202
for more detail on this issue.
    ])
    ;;
  *)
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
    ;;
  esac
fi
])

PHP_ARG_ENABLE(pdo, whether to enable PDO support,
[  --enable-pdo=yes         Enable PHP Data Objects support (recommended)], yes)

if test "$PHP_PDO" != "no"; then
  if test "$ext_shared" = "yes" ; then
    case $host_alias in
      *darwin*)
        ext_shared=no
        ;;
    esac
  fi
  PHP_NEW_EXTENSION(pdo, pdo.c pdo_dbh.c pdo_stmt.c pdo_sql_parser.c pdo_sqlstate.c, $ext_shared)
  PHP_ADD_MAKEFILE_FRAGMENT
fi
