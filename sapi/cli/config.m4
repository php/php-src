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
  case $host_alias in
  *darwin*)
    BUILD_CLI="\$(CC) \$(COMMON_FLAGS) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_CLI_OBJS:.lo=.o) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o sapi/cli/php"
    ;;
  *)
    BUILD_CLI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(COMMON_FLAGS) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o sapi/cli/php"
    ;;
  esac
  INSTALL_CLI="\$(INSTALL) -m 0755 sapi/cli/php \$(INSTALL_ROOT)\$(bindir)/php"
  PHP_SUBST(BUILD_CLI)
  PHP_SUBST(INSTALL_CLI)
else
  PHP_DISABLE_CLI
fi

AC_MSG_RESULT($PHP_SAPI_CLI)
