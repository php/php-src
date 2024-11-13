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
      PHP_EMBED_TYPE=shared
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
    PHP_INSTALL_HEADERS([sapi/embed], [php_embed.h])
  ])
else
  AC_MSG_RESULT([no])
fi
