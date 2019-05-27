PHP_ARG_WITH([zip],
  [for zip archive read/write support],
  [AS_HELP_STRING([--with-zip],
    [Include Zip read/write support])])

if test "$PHP_ZIP" != "no"; then
  PKG_CHECK_MODULES([LIBZIP], [libzip >= 0.11])

  PHP_EVAL_INCLINE($LIBZIP_CFLAGS)
  LIBZIP_LIBDIR=`$PKG_CONFIG --variable=libdir libzip`

  dnl Could not think of a simple way to check libzip for overwrite support
  PHP_CHECK_LIBRARY(zip, zip_open,
  [
    PHP_ADD_LIBRARY_WITH_PATH(zip, $LIBZIP_LIBDIR, ZIP_SHARED_LIBADD)
    AC_DEFINE(HAVE_LIBZIP,1,[ ])
  ], [
    AC_MSG_ERROR(could not find usable libzip)
  ], [
    -L$LIBZIP_LIBDIR
  ])

  PHP_CHECK_LIBRARY(zip, zip_file_set_encryption,
  [
    PHP_ADD_LIBRARY_WITH_PATH(zip, $LIBZIP_LIBDIR, ZIP_SHARED_LIBADD)
    AC_DEFINE(HAVE_ENCRYPTION, 1, [Libzip >= 1.2.0 with encryption support])
  ], [
    AC_MSG_WARN(Libzip >= 1.2.0 needed for encryption support)
  ], [
    -L$LIBZIP_LIBDIR
  ])

  PHP_CHECK_LIBRARY(zip, zip_libzip_version,
  [
    AC_DEFINE(HAVE_LIBZIP_VERSION, 1, [Libzip >= 1.3.1 with zip_libzip_version function])
  ], [
  ], [
    -L$LIBZIP_LIBDIR
  ])

  AC_DEFINE(HAVE_ZIP,1,[ ])

  PHP_ZIP_SOURCES="php_zip.c zip_stream.c"
  PHP_NEW_EXTENSION(zip, $PHP_ZIP_SOURCES, $ext_shared)

  PHP_SUBST(ZIP_SHARED_LIBADD)

  dnl so we always include the known-good working hack.
  PHP_ADD_MAKEFILE_FRAGMENT
fi
