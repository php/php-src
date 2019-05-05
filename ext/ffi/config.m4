dnl config.m4 for extension FFI

PHP_ARG_WITH([ffi],
  [for FFI support],
  [AS_HELP_STRING([--with-ffi],
    [Include FFI support])])

if test "$PHP_FFI" != "no"; then
  PKG_CHECK_MODULES([FFI], [libffi])

  AC_CHECK_TYPES(long double)

  PHP_CHECK_LIBRARY(ffi, ffi_call,
  [
    PHP_EVAL_INCLINE($FFI_CFLAGS)
    PHP_EVAL_LIBLINE($FFI_LIBS, FFI_SHARED_LIBADD)
    AC_DEFINE(HAVE_FFI,1,[ Have ffi support ])
  ], [
    AC_MSG_ERROR(FFI module requires libffi)
  ])

  PHP_NEW_EXTENSION(ffi, ffi.c ffi_parser.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(FFI_SHARED_LIBADD)
fi
