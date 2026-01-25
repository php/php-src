PHP_ARG_ENABLE([embed],,
  [AS_HELP_STRING([[--enable-embed[=TYPE]]],
    [Enable building of embedded SAPI library TYPE is either
    'shared' or 'static' or 'main'. [TYPE=shared]])],
  [no],
  [no])

PHP_ARG_WITH([embed-main],,
  [AS_HELP_STRING([[--enable-embed-main[=FILE]]],
    [Enable building of embedded SAPI as a program with embedded main
                          [TYPE=no]])],
  [no],
  [no])

PHP_ARG_WITH([embed-link],,
  [AS_HELP_STRING([[--enable-embed-link[=FILE]]],
    [Enable creation of symlink to embedded code
                          [TYPE=no]])],
  [no],
  [no])

AC_MSG_CHECKING([for embedded SAPI library support])

if test "$PHP_EMBED" != "no" || test "$PHP_EMBED_MAIN" != "no"; then

  if test "$PHP_EMBED" == "main"; then
    if test "$PHP_EMBED_MAIN" == "no"; then
      AC_MSG_ERROR([--with-embed-main is required for --enable-embed=main])  
    fi
  elif test "$PHP_EMBED_MAIN" != "no"; then
    PHP_EMBED=main
  fi

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
    [main], [
      AC_MSG_RESULT([main])
      AC_PATH_PROG([XXD], [xxd])
      if test ! -x "$XXD"; then
        AC_MSG_ERROR([xxd not found, required for --with-embed-main])
      fi
      AC_MSG_CHECKING([for access to $PHP_EMBED_MAIN])
      if test ! -f "$PHP_EMBED_MAIN"; then
        AC_MSG_ERROR([--with-embed-main argument "$PHP_EMBED_MAIN" not found])
      else
        AC_MSG_RESULT([ok])
      fi

      AC_MSG_CHECKING([generating sapi/embed/php_embed_main.h])
      $XXD -i -n php_embed_main "$PHP_EMBED_MAIN" sapi/embed/php_embed_main.h
      if test $? -ne 0; then
        AC_MSG_ERROR([failed])
      else
        AC_MSG_RESULT([ok])
      fi

      AC_MSG_CHECKING([proceeding to final configure of embed])
      LIBPHP_CFLAGS="-static"
      PHP_EMBED_TYPE=static
      INSTALL_IT="\
        \$(mkinstalldirs) \$(INSTALL_ROOT)\$(orig_libdir); \
        \$(INSTALL) -m 0644 \$(SAPI_STATIC) \$(INSTALL_ROOT)\$(orig_libdir); \
        \$(mkinstalldirs) \$(INSTALL_ROOT)\$(bindir); \
        \$(LIBTOOL) --tag=CC --mode=compile \$(CC) \
          \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(INCLUDES) -DHAVE_EMBED_MAIN \
            -c sapi/embed/php_embed.c -o sapi/embed/php_embed_main.lo; \
        \$(LIBTOOL) --tag=CC --mode=link \$(CC) \
          -export-dynamic \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \
            sapi/embed/php_embed_main.lo \
            \$(PHP_GLOBAL_OBJS) \$(PHP_SAPI_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) \
          -o \$(INSTALL_ROOT)\$(bindir)/php-embed-main"
        if test "$PHP_EMBED_LINK" != "no"; then
          AC_DEFINE_UNQUOTED([PHP_EMBED_LINK], ["$PHP_EMBED_LINK"], [PHP_EMBED_LINK])
        fi
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
