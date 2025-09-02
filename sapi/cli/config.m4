PHP_ARG_ENABLE([cli],
  [for CLI build],
  [AS_HELP_STRING([--disable-cli],
    [Disable building CLI version of PHP (this forces --without-pear)])],
  [yes],
  [no])

if test "$PHP_CLI" != "no"; then
  AC_CHECK_FUNCS([setproctitle])

  AC_CHECK_HEADERS([sys/pstat.h])

  AC_CACHE_CHECK([for PS_STRINGS], [php_cv_var_PS_STRINGS],
    [AC_LINK_IFELSE([AC_LANG_PROGRAM([
      #include <machine/vmparam.h>
      #include <sys/exec.h>
    ],
    [
      PS_STRINGS->ps_nargvstr = 1;
      PS_STRINGS->ps_argvstr = "foo";
    ])],
    [php_cv_var_PS_STRINGS=yes],
    [php_cv_var_PS_STRINGS=no])])
  AS_VAR_IF([php_cv_var_PS_STRINGS], [yes],
    [AC_DEFINE([HAVE_PS_STRINGS], [], [Define if the PS_STRINGS exists.])])

  PHP_ADD_MAKEFILE_FRAGMENT([$abs_srcdir/sapi/cli/Makefile.frag])

  dnl Set filename.
  SAPI_CLI_PATH=sapi/cli/php

  dnl Select SAPI.
  PHP_SELECT_SAPI([cli],
    [program],
    [php_cli.c php_http_parser.c php_cli_server.c ps_title.c php_cli_process_title.c],
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])

  AS_CASE([$host_alias],
    [*aix*], [
      AS_VAR_IF([php_sapi_module], [shared], [
        BUILD_CLI="echo '\#! .' > php.sym && echo >>php.sym && nm -BCpg \`echo \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_CLI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/.libs\/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> php.sym && \$(LIBTOOL) --tag=CC --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:php.sym \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
      ], [
        BUILD_CLI="echo '\#! .' > php.sym && echo >>php.sym && nm -BCpg \`echo \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_CLI_OBJS) | sed 's/\([A-Za-z0-9_]*\)\.lo/\1.o/g'\` | \$(AWK) '{ if (((\$\$2 == \"T\") || (\$\$2 == \"D\") || (\$\$2 == \"B\")) && (substr(\$\$3,1,1) != \".\")) { print \$\$3 } }' | sort -u >> php.sym && \$(LIBTOOL) --tag=CC --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) -Wl,-brtl -Wl,-bE:php.sym \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_CLI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
      ])
    ],
    [*darwin*], [
      BUILD_CLI="\$(CC) \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(NATIVE_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_BINARY_OBJS:.lo=.o) \$(PHP_CLI_OBJS:.lo=.o) \$(PHP_FRAMEWORKS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    ], [
      BUILD_CLI="\$(LIBTOOL) --tag=CC --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS:.lo=.o) \$(PHP_BINARY_OBJS:.lo=.o) \$(PHP_CLI_OBJS:.lo=.o) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_CLI_PATH)"
    ])

  dnl Set executable for tests.
  PHP_EXECUTABLE="\$(top_builddir)/\$(SAPI_CLI_PATH)"

  PHP_SUBST([PHP_EXECUTABLE])
  PHP_SUBST([SAPI_CLI_PATH])
  PHP_SUBST([BUILD_CLI])

  AC_CONFIG_FILES([sapi/cli/php.1])

  PHP_INSTALL_HEADERS([sapi/cli], [cli.h])
fi
