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
  PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/cli/Makefile.frag,$abs_srcdir/sapi/cli,sapi/cli)
  SAPI_CLI_PATH=sapi/cli/php
  PHP_SUBST(SAPI_CLI_PATH)

  case $host_alias in
  *darwin*)
    BUILD_CLI="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_CLI_OBJS:.lo=.o) \$(PHP_FRAMEWORKS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    ;;
  *cygwin*)
    SAPI_CLI_PATH=sapi/cli/php.exe
    BUILD_CLI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    ;;
  *)
    BUILD_CLI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    ;;
  esac
  INSTALL_CLI="\$(mkinstalldirs) \$(INSTALL_ROOT)\$(bindir); \$(INSTALL) -m 0755 \$(SAPI_CLI_PATH) \$(INSTALL_ROOT)\$(bindir)/php"

  PHP_SUBST(BUILD_CLI)
  PHP_SUBST(INSTALL_CLI)
  PHP_OUTPUT(sapi/cli/php.1)
fi

AC_MSG_RESULT($PHP_SAPI_CLI)
