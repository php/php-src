dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for CLI build)

AC_ARG_ENABLE(cli,
[  --disable-cli           Disable building CLI version of PHP
                          (this forces --without-pear).],
[
  PHP_SAPI_CLI=$enableval
],[
  PHP_SAPI_CLI=yes
])

if test "$PHP_SAPI_CLI" != "no"; then
  INSTALL_CLI="\$(INSTALL) -m 0755 sapi/cli/php \$(INSTALL_ROOT)\$(bindir)/php"
  PHP_SUBST(INSTALL_CLI)
else
  PHP_DISABLE_CLI
fi

AC_MSG_RESULT($PHP_SAPI_CLI)
