dnl Configure options
dnl
PHP_ARG_ENABLE([url],
  [for URL support],
  [AS_HELP_STRING([--enable-url],
    [Enable URL support])])

if test "$PHP_URL" != "no"; then
    PHP_LEXBOR_CFLAGS="-I$abs_srcdir/ext/dom/lexbor -DLEXBOR_STATIC"

    AC_DEFINE(HAVE_URL, 1, [Whether the url extension is enabled])
    PHP_INSTALL_HEADERS([ext/url], [php_url.h])
    PHP_ADD_EXTENSION_DEP(url, dom)
    PHP_NEW_EXTENSION(url, php_url.c, $ext_shared,,$PHP_LEXBOR_CFLAGS)
fi
