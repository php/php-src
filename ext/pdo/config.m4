dnl config.m4 for extension pdo
dnl vim:se ts=2 sw=2 et:

AC_DEFUN([PHP_PDO_PEAR_CHECK],[
  pdo_running_under_pear=0
  case `pwd` in
    /var/tmp/pear-build-*)
      pdo_running_under_pear=1
      ;;
  esac

  if test "$pdo_running_under_pear$PHP_PEAR_VERSION" = "1"; then
    # we're running in an environment that smells like pear,
    # and the PHP_PEAR_VERSION env var is not set.  That implies
    # that we're running under a slightly broken pear installer
    AC_MSG_ERROR([
PDO requires that you upgrade your PEAR installer tools. Please
do so now by running:

  % sudo pear upgrade pear

or by manually downloading and installing PEAR version 1.3.5 or higher.

Once you've upgraded, please re-try your PDO install.
    ])
  fi
])

PHP_ARG_ENABLE(pdo, whether to enable PDO support,
[  --disable-pdo           Disable PHP Data Objects support], yes)

if test "$PHP_PDO" != "no"; then

  dnl Make sure $PHP_PDO is 'yes' when it's not 'no' :)
  PHP_PDO=yes

  PHP_PDO_PEAR_CHECK

  PHP_NEW_EXTENSION(pdo, pdo.c pdo_dbh.c pdo_stmt.c pdo_sql_parser.c pdo_sqlstate.c, $ext_shared)
  ifdef([PHP_ADD_EXTENSION_DEP],
  [
    PHP_ADD_EXTENSION_DEP(pdo, spl, true)
  ])

  ifdef([PHP_INSTALL_HEADERS],
  [
    dnl Sadly, this is a complete NOP for pecl extensions
    PHP_INSTALL_HEADERS(ext/pdo, [php_pdo.h php_pdo_driver.h php_pdo_error.h])
  ])

  dnl so we always include the known-good working hack.
  PHP_ADD_MAKEFILE_FRAGMENT
fi
