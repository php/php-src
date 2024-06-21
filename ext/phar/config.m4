PHP_ARG_ENABLE([phar],
  [for phar archive support],
  [AS_HELP_STRING([--disable-phar],
    [Disable phar support])],
  [yes])

PHP_ARG_WITH([phar-ssl],
  [whether to explicitly enable SSL support for phar],
  [AS_HELP_STRING([--with-phar-ssl],
    [Explicitly enable SSL support in phar extension when building without
    openssl extension. If openssl extension is enabled at the configure step,
    SSL is enabled implicitly.])],
  [no],
  [no])

if test "$PHP_PHAR" != "no"; then
  PHP_NEW_EXTENSION(phar, util.c tar.c zip.c stream.c func_interceptors.c dirstream.c phar.c phar_object.c phar_path_check.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)

  dnl Empty variable means 'no' (for phpize builds).
  AS_VAR_IF([PHP_OPENSSL],, [PHP_OPENSSL=no])

  AS_IF([test "$PHP_OPENSSL" != no || test "$PHP_PHAR_SSL" != no], [dnl
    PHP_SETUP_OPENSSL([PHAR_SHARED_LIBADD],
      [AC_DEFINE([PHAR_HAVE_OPENSSL], [1],
        [Define to 1 if phar extension has native OpenSSL support.])])
    PHP_SUBST([PHAR_SHARED_LIBADD])
    AC_MSG_NOTICE([phar SSL support enabled])
  ])

  PHP_ADD_EXTENSION_DEP(phar, hash, true)
  PHP_ADD_EXTENSION_DEP(phar, spl, true)
  PHP_ADD_MAKEFILE_FRAGMENT

  PHP_INSTALL_HEADERS([ext/phar], [php_phar.h])

  PHP_OUTPUT(ext/phar/phar.1 ext/phar/phar.phar.1)
fi
