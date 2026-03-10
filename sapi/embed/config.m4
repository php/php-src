PHP_ARG_ENABLE([embed],,
  [AS_HELP_STRING([[--enable-embed[=TYPE]]],
    [Enable building of embedded SAPI library TYPE is either
    'shared' or 'static'. [TYPE=shared]])],
  [no],
  [no])

AC_MSG_CHECKING([for embedded SAPI library support])

if test "$PHP_EMBED" != "no"; then
  AS_CASE([$PHP_EMBED],
    [yes|shared], [
      LIBPHP_CFLAGS="-shared"
      AS_CASE(["$host_alias"], [*darwin*], [
        SAPI_SHARED="libs/libphp.dylib"
        PHP_EMBED_TYPE=shared-dylib
      ], [PHP_EMBED_TYPE=shared])
      INSTALL_IT="\$(mkinstalldirs) \$(INSTALL_ROOT)\$(orig_libdir); \$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)\$(orig_libdir)"
    ],
    [static], [
      LIBPHP_CFLAGS="-static"
      PHP_EMBED_TYPE=static
      INSTALL_IT="\$(mkinstalldirs) \$(INSTALL_ROOT)\$(orig_libdir); \$(INSTALL) -m 0644 $SAPI_STATIC \$(INSTALL_ROOT)\$(orig_libdir)"
    ],
    [PHP_EMBED_TYPE=])

  AS_VAR_IF([PHP_EMBED_TYPE],, [AC_MSG_RESULT([no])], [
    AC_MSG_RESULT([$PHP_EMBED_TYPE])
    PHP_SUBST([LIBPHP_CFLAGS])
    AC_SUBST([PHP_EMBED_TYPE])
    PHP_SELECT_SAPI([embed],
      [$PHP_EMBED_TYPE],
      [php_embed.c],
      [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])

    dnl Feature checks for CLI sources.
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

    dnl When CLI SAPI is also enabled, these are already in PHP_GLOBAL_OBJS.
    if test "$PHP_CLI" = "no"; then
      PHP_ADD_SOURCES([main/cli],
        [php_cli.c php_http_parser.c php_cli_server.c ps_title.c php_cli_process_title.c],
        [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1],
        [sapi])
    fi

    PHP_INSTALL_HEADERS([sapi/embed], [php_embed.h])
  ])
else
  AC_MSG_RESULT([no])
fi
