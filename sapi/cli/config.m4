dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for CLI build)

AC_ARG_ENABLE(cli,
[  --enable-cli            Enable building CLI version of PHP.],
[
  PHP_SAPI_CLI=$enableval
],[
  PHP_SAPI_CLI=no
])

if test "$PHP_SAPI_CLI" != "yes"; then
  PHP_DISABLE_CLI
fi

AC_MSG_RESULT($PHP_SAPI_CLI)
