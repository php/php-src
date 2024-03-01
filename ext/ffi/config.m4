PHP_ARG_WITH([ffi],
  [for FFI support],
  [AS_HELP_STRING([--with-ffi],
    [Include FFI support])])

if test "$PHP_FFI" != "no"; then
  PKG_CHECK_MODULES([FFI], [libffi >= 3.0.11])

  PHP_EVAL_INCLINE($FFI_CFLAGS)
  PHP_EVAL_LIBLINE($FFI_LIBS, FFI_SHARED_LIBADD)

  AC_DEFINE(HAVE_FFI, 1, [Have ffi support])

  AC_CHECK_TYPES(long double)

  AC_DEFUN([PHP_FFI_CHECK_DECL],
    [AC_CHECK_DECL([$1],
      [AC_DEFINE(AS_TR_CPP([HAVE_$1]), [1],
        [Whether libffi supports the '$1' calling convention.])],,
      [#include <ffi.h>])])

  PHP_FFI_CHECK_DECL([FFI_FASTCALL])
  PHP_FFI_CHECK_DECL([FFI_THISCALL])
  PHP_FFI_CHECK_DECL([FFI_STDCALL])
  PHP_FFI_CHECK_DECL([FFI_PASCAL])
  PHP_FFI_CHECK_DECL([FFI_REGISTER])
  PHP_FFI_CHECK_DECL([FFI_MS_CDECL])
  PHP_FFI_CHECK_DECL([FFI_SYSV])

  PHP_NEW_EXTENSION(ffi, ffi.c ffi_parser.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(FFI_SHARED_LIBADD)
fi
