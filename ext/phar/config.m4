PHP_ARG_ENABLE([phar],
  [for phar archive support],
  [AS_HELP_STRING([--disable-phar],
    [Disable phar support])],
  [yes])

dnl Empty variable means 'no' (for phpize builds).
AS_VAR_IF([PHP_OPENSSL],, [PHP_OPENSSL=no])

PHP_ARG_WITH([phar-ssl],
  [whether to enable native OpenSSL support for phar],
  [AS_HELP_STRING([--with-phar-ssl],
    [Explicitly enable SSL support in phar extension through the OpenSSL library
    when building without openssl extension or when using phpize. If the openssl
    extension is enabled at the configure step (--with-openssl), SSL is enabled
    implicitly regardless of this option.])],
  [$PHP_OPENSSL],
  [no])

if test "$PHP_PHAR" != "no"; then
  PHP_NEW_EXTENSION([phar], m4_normalize([
      dirstream.c
      func_interceptors.c
      phar_object.c
      phar_path_check.c
      phar.c
      stream.c
      tar.c
      util.c
      zip.c
    ]),
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])

  AS_VAR_IF([PHP_PHAR_SSL], [no],, [
    PHP_SETUP_OPENSSL([PHAR_SHARED_LIBADD],
      [AC_DEFINE([PHAR_HAVE_OPENSSL], [1],
        [Define to 1 if phar extension has native OpenSSL support.])])
    PHP_SUBST([PHAR_SHARED_LIBADD])
  ])

  PHP_ADD_EXTENSION_DEP(phar, hash)
  PHP_ADD_EXTENSION_DEP(phar, spl)
  PHP_ADD_MAKEFILE_FRAGMENT

  PHP_INSTALL_HEADERS([ext/phar], [php_phar.h])

  AC_CONFIG_FILES([
    $ext_dir/phar.1
    $ext_dir/phar.phar.1
  ])
fi
