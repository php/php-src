dnl config.m4 for extension zip

PHP_ARG_ENABLE(zip, for zip archive read/writesupport,
[  --enable-zip            Include Zip read/write support])

PHP_ARG_WITH(libzip, libzip,
[  --with-libzip[=DIR]       ZIP: use libzip], yes, no)

if test "$PHP_ZIP" != "no"; then

  PHP_ZIP_SOURCES="php_zip.c zip_stream.c"

  if test "$PHP_LIBZIP" != "no"; then

    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)

    AC_MSG_CHECKING(for libzip)
    if test -r $PHP_LIBZIP/include/zip.h; then
      LIBZIP_CFLAGS="-I$PHP_LIBZIP/include"
      LIBZIP_LIBDIR="$PHP_LIBZIP/$PHP_LIBDIR"
      AC_MSG_RESULT(from option: found in $PHP_LIBZIP)

    elif test -x "$PKG_CONFIG" && $PKG_CONFIG --exists libzip; then
      if $PKG_CONFIG libzip --atleast-version 0.11; then
        LIBZIP_CFLAGS=`$PKG_CONFIG libzip --cflags`
        LIBZIP_LIBDIR=`$PKG_CONFIG libzip --variable=libdir`
        LIBZIP_VERSON=`$PKG_CONFIG libzip --modversion`
        AC_MSG_RESULT(from pkgconfig: version $LIBZIP_VERSON found in $LIBZIP_LIBDIR)
      else
        AC_MSG_ERROR(system libzip must be upgraded to version >= 0.11)
      fi

    else
      for i in /usr/local /usr; do
        if test -r $i/include/zip.h; then
          LIBZIP_CFLAGS="-I$i/include"
          LIBZIP_LIBDIR="$i/$PHP_LIBDIR"
          AC_MSG_RESULT(in default path: found in $i)
          break
        fi
      done
    fi

    if test -z "$LIBZIP_LIBDIR"; then
      AC_MSG_RESULT(not found)
      AC_MSG_ERROR(Please reinstall the libzip distribution)
    fi

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
    PHP_NEW_EXTENSION(zip, $PHP_ZIP_SOURCES, $ext_shared,, $LIBZIP_CFLAGS)
  else
    AC_MSG_ERROR([libzip is no longer bundled: install libzip version >= 0.11 (1.3.0 recommended for encryption and bzip2 support)])
  fi

  PHP_SUBST(ZIP_SHARED_LIBADD)

  dnl so we always include the known-good working hack.
  PHP_ADD_MAKEFILE_FRAGMENT
fi
