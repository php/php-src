dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for CLI build)

AC_ARG_ENABLE(cli,
[  --disable-cli           Disable building CLI version of PHP
                          (this forces --without-pear)],
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
  *aix*)
    BUILD_CLI="echo '\#! .' > php.sym && echo >>php.sym && nm -BCpg \`echo \$(PHP_GLOBAL_OBJS) \$(PHP_CLI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> php.sym && \$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:php.sym \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    ;;
  *darwin*)
    BUILD_CLI="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_CLI_OBJS:.lo=.o) \$(PHP_FRAMEWORKS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    ;;
  *netware*)
    BUILD_CLI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -Lnetware -lphp6lib -o \$(SAPI_CLI_PATH)"
    ;;
  *)
    BUILD_CLI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    ;;
  esac
  INSTALL_CLI="\$(mkinstalldirs) \$(INSTALL_ROOT)\$(bindir); \$(INSTALL) -m 0755 \$(SAPI_CLI_PATH) \$(INSTALL_ROOT)\$(bindir)/\$(program_prefix)php\$(program_suffix)\$(EXEEXT)"

  PHP_SUBST(BUILD_CLI)
  PHP_SUBST(INSTALL_CLI)
  PHP_OUTPUT(sapi/cli/php.1)
fi

AC_MSG_RESULT($PHP_SAPI_CLI)
