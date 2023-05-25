dnl Configure options
dnl
PHP_ARG_ENABLE([url],
  [for URL support],
  [AS_HELP_STRING([--enable-url],
    [Enable URL support])])

if test "$PHP_URL" != "no"; then
    AC_DEFINE(HAVE_URL, 1, [Whether the url extension is enabled])
    PHP_NEW_EXTENSION(url,
        url.c,
        $ext_shared,,-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1,cxx)

    PHP_URL_CXX_SOURCES="ada_url.cpp ada/ada.cpp"

    PHP_REQUIRE_CXX()
    PHP_CXX_COMPILE_STDCXX(17, mandatory, PHP_URL_STDCXX)
    PHP_URL_CXX_FLAGS="-Wno-undefined-inline -I@ext_srcdir@/ada -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1 $PHP_URL_STDCXX"
    PHP_ADD_SOURCES(PHP_EXT_DIR(url), $PHP_URL_CXX_SOURCES, $PHP_URL_CXX_FLAGS)

    PHP_ADD_BUILD_DIR([$ext_builddir/ada], 1)

    PHP_ADD_INCLUDE([$ext_srcdir/ada])

    PHP_INSTALL_HEADERS([ext/url], [php_url.h ada_url.h ada/ada.h ada/ada_c.h])
fi
