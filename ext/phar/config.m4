PHP_ARG_ENABLE([phar],
  [for phar archive support],
  [AS_HELP_STRING([--disable-phar],
    [Disable phar support])],
  [yes])

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
  AC_MSG_CHECKING([for phar openssl support])
  if test "$PHP_OPENSSL_SHARED" = "yes"; then
    AC_MSG_RESULT([no (shared openssl)])
  else
    if test "$PHP_OPENSSL" = "yes"; then
      AC_MSG_RESULT([yes])
      AC_DEFINE(PHAR_HAVE_OPENSSL,1,[ ])
    else
      AC_MSG_RESULT([no])
    fi
  fi
  PHP_ADD_EXTENSION_DEP(phar, hash)
  PHP_ADD_EXTENSION_DEP(phar, spl)
  PHP_ADD_MAKEFILE_FRAGMENT

  PHP_INSTALL_HEADERS([ext/phar], [php_phar.h])

  AC_CONFIG_FILES([
    $ext_dir/phar.1
    $ext_dir/phar.phar.1
  ])
fi
