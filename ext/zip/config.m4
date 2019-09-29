PHP_ARG_WITH([zip],
  [for zip archive read/write support],
  [AS_HELP_STRING([--with-zip],
    [Include Zip read/write support])])

if test "$PHP_ZIP" != "no"; then
  PKG_CHECK_MODULES([LIBZIP], [libzip >= 0.11])

  PHP_EVAL_INCLINE($LIBZIP_CFLAGS)
  PHP_EVAL_LIBLINE($LIBZIP_LIBS, ZIP_SHARED_LIBADD)

  PHP_CHECK_LIBRARY(zip, zip_file_set_encryption,
  [
    AC_DEFINE(HAVE_ENCRYPTION, 1, [Libzip >= 1.2.0 with encryption support])
  ], [
    AC_MSG_WARN(Libzip >= 1.2.0 needed for encryption support)
  ], [
    $LIBZIP_LIBS
  ])

  PHP_CHECK_LIBRARY(zip, zip_libzip_version,
  [
    AC_DEFINE(HAVE_LIBZIP_VERSION, 1, [Libzip >= 1.3.1 with zip_libzip_version function])
  ], [
  ], [
    $LIBZIP_LIBS
  ])

  AC_DEFINE(HAVE_ZIP,1,[ ])

  PHP_ZIP_SOURCES="php_zip.c zip_stream.c"
  PHP_NEW_EXTENSION(zip, $PHP_ZIP_SOURCES, $ext_shared)

  PHP_SUBST(ZIP_SHARED_LIBADD)

  dnl so we always include the known-good working hack.
  PHP_ADD_MAKEFILE_FRAGMENT
fi
