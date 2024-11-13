PHP_ARG_WITH([zip],
  [for zip archive read/write support],
  [AS_HELP_STRING([--with-zip],
    [Include Zip read/write support])])

if test "$PHP_ZIP" != "no"; then
  PKG_CHECK_MODULES([LIBZIP], [libzip >= 0.11 libzip != 1.3.1 libzip != 1.7.0])

  PHP_EVAL_INCLINE([$LIBZIP_CFLAGS])
  PHP_EVAL_LIBLINE([$LIBZIP_LIBS], [ZIP_SHARED_LIBADD])

  PHP_CHECK_LIBRARY([zip], [zip_file_set_mtime],
    [AC_DEFINE([HAVE_SET_MTIME], [1],
      [Define to 1 if libzip library has the 'zip_file_set_mtime' function
      (available since 1.0.0).])],
    [AC_MSG_WARN([Libzip >= 1.0.0 needed for setting mtime])],
    [$LIBZIP_LIBS])

  PHP_CHECK_LIBRARY([zip], [zip_file_set_encryption],
    [AC_DEFINE([HAVE_ENCRYPTION], [1],
      [Define to 1 if libzip library has encryption support (available since
      1.2.0).])],
    [AC_MSG_WARN([Libzip >= 1.2.0 needed for encryption support])],
    [$LIBZIP_LIBS])

  PHP_CHECK_LIBRARY([zip], [zip_libzip_version],
    [AC_DEFINE([HAVE_LIBZIP_VERSION], [1],
      [Define to 1 if libzip library has the 'zip_libzip_version' function
      (available since 1.3.1).])],
    [],
    [$LIBZIP_LIBS])

  PHP_CHECK_LIBRARY([zip], [zip_register_progress_callback_with_state],
    [AC_DEFINE([HAVE_PROGRESS_CALLBACK], [1],
      [Define to 1 if libzip library has the
      'zip_register_progress_callback_with_state' function (available since
      1.3.0).])],
    [],
    [$LIBZIP_LIBS])

  PHP_CHECK_LIBRARY([zip], [zip_register_cancel_callback_with_state],
    [AC_DEFINE([HAVE_CANCEL_CALLBACK], [1],
      [Define to 1 if libzip library has the
      'zip_register_cancel_callback_with_state' function (available since
      1.6.0).])],
    [],
    [$LIBZIP_LIBS])

  PHP_CHECK_LIBRARY([zip], [zip_compression_method_supported],
    [AC_DEFINE([HAVE_METHOD_SUPPORTED], [1],
      [Define to 1 if libzip library has 'zip_*_method_supported' functions
      (available since 1.7.0).])],
    [],
    [$LIBZIP_LIBS])

  AC_DEFINE([HAVE_ZIP], [1],
    [Define to 1 if the PHP extension 'zip' is available.])

  PHP_NEW_EXTENSION([zip], [php_zip.c zip_stream.c], [$ext_shared])
  PHP_ADD_EXTENSION_DEP(zip, pcre)

  PHP_SUBST([ZIP_SHARED_LIBADD])
fi
