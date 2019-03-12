dnl config.m4 for extension FFI

PHP_ARG_WITH([ffi],
  [for FFI support],
  [AS_HELP_STRING([--with-ffi],
    [Include FFI support])])

if test "$PHP_FFI" != "no"; then
  if test -r $PHP_FFI/include/ffi.h; then
    FFI_INCDIR=$PHP_FFI/include
    FFI_LIBDIR=$PHP_FFI
  else
    dnl First try to find pkg-config
    if test -z "$PKG_CONFIG"; then
      AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
    fi

    dnl If pkg-config is installed, try using it
    if test -x "$PKG_CONFIG" && "$PKG_CONFIG" --exists libffi; then
      FFI_VER=`"$PKG_CONFIG" --modversion libffi`
      FFI_INCDIR=`"$PKG_CONFIG" --variable=includedir libffi`
      FFI_LIBDIR=`"$PKG_CONFIG" --variable=libdir libffi`
      AC_MSG_CHECKING(for libffi)
      AC_MSG_RESULT(found version $FFI_VER)
    else
      AC_MSG_CHECKING(for libffi in default path)
      for i in /usr/local /usr; do
        if test -r $i/include/ffi.h; then
          FFI_DIR=$i
          AC_MSG_RESULT(found in $i)
          break
        fi
      done

      if test -z "$FFI_DIR"; then
        AC_MSG_RESULT(not found)
        AC_MSG_ERROR(Please reinstall the libffi distribution)
      fi

      FFI_INCDIR="$FFI_DIR/include"
      FFI_LIBDIR="$FFI_DIR/$PHP_LIBDIR"
    fi
  fi

  AC_CHECK_TYPES(long double)

  PHP_CHECK_LIBRARY(ffi, ffi_call,
  [
    PHP_ADD_INCLUDE($FFI_INCDIR)
    PHP_ADD_LIBRARY_WITH_PATH(ffi, $FFI_LIBDIR, FFI_SHARED_LIBADD)
    AC_DEFINE(HAVE_FFI,1,[ Have ffi support ])
  ], [
    AC_MSG_ERROR(FFI module requires libffi)
  ], [
    -L$FFI_LIBDIR
  ])

  PHP_NEW_EXTENSION(ffi, ffi.c ffi_parser.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(FFI_SHARED_LIBADD)
fi
