PHP_ARG_ENABLE([filter],
  [whether to enable input filter support],
  [AS_HELP_STRING([--disable-filter],
    [Disable input filter support])],
  [yes])

if test "$PHP_FILTER" != "no"; then
  PHP_NEW_EXTENSION(filter, filter.c sanitizing_filters.c logical_filters.c callback_filter.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)

  PHP_INSTALL_HEADERS([ext/filter], [php_filter.h])
  PHP_ADD_EXTENSION_DEP(filter, pcre)
fi
