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

    dnl Reuse CLI object files (excluding php_cli_main.lo) for do_php_cli()
    PHP_SAPI_OBJS="$PHP_SAPI_OBJS sapi/cli/php_cli.lo sapi/cli/php_http_parser.lo sapi/cli/php_cli_server.lo sapi/cli/ps_title.lo sapi/cli/php_cli_process_title.lo"

    PHP_INSTALL_HEADERS([sapi/embed], [php_embed.h])
  ])
else
  AC_MSG_RESULT([no])
fi
