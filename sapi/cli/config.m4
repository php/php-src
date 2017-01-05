dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(cli,,
[  --disable-cli           Disable building CLI version of PHP
                          (this forces --without-pear)], yes, no)

AC_CHECK_FUNCS(setproctitle)

AC_CHECK_HEADERS([sys/pstat.h])

AC_CACHE_CHECK([for PS_STRINGS], [cli_cv_var_PS_STRINGS],
[AC_TRY_LINK(
[#include <machine/vmparam.h>
#include <sys/exec.h>
],
[PS_STRINGS->ps_nargvstr = 1;
PS_STRINGS->ps_argvstr = "foo";],
[cli_cv_var_PS_STRINGS=yes],
[cli_cv_var_PS_STRINGS=no])])
if test "$cli_cv_var_PS_STRINGS" = yes ; then
  AC_DEFINE([HAVE_PS_STRINGS], [], [Define to 1 if the PS_STRINGS thing exists.])
fi

AC_MSG_CHECKING(for CLI build)
if test "$PHP_CLI" != "no"; then
  PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/cli/Makefile.frag)

  dnl Set filename
  SAPI_CLI_PATH=sapi/cli/php

  dnl Select SAPI
  PHP_SELECT_SAPI(cli, program, php_cli.c php_http_parser.c php_cli_server.c ps_title.c php_cli_process_title.c, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1, '$(SAPI_CLI_PATH)')

  case $host_alias in
  *aix*)
    if test "$php_sapi_module" = "shared"; then
      BUILD_CLI="echo '\#! .' > php.sym && echo >>php.sym && nm -BCpg \`echo \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_CLI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/.libs\/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> php.sym && \$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:php.sym \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    else
      BUILD_CLI="echo '\#! .' > php.sym && echo >>php.sym && nm -BCpg \`echo \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_CLI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> php.sym && \$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:php.sym \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    fi
    ;;
  *darwin*)
    BUILD_CLI="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_BINARY_OBJS:.lo=.o) \$(PHP_CLI_OBJS:.lo=.o) \$(PHP_FRAMEWORKS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    ;;
  *netware*)
    BUILD_CLI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_BINARY_OBJS) \$(PHP_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -Lnetware -lphp7lib -o \$(SAPI_CLI_PATH)"
    ;;
  *)
    BUILD_CLI="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    ;;
  esac

  dnl Set executable for tests
  PHP_EXECUTABLE="\$(top_builddir)/\$(SAPI_CLI_PATH)"
  PHP_SUBST(PHP_EXECUTABLE)

  dnl Expose to Makefile
  PHP_SUBST(SAPI_CLI_PATH)
  PHP_SUBST(BUILD_CLI)

  PHP_OUTPUT(sapi/cli/php.1)

  PHP_INSTALL_HEADERS([sapi/cli/cli.h])
fi
AC_MSG_RESULT($PHP_CLI)
