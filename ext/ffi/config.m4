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

  AC_CACHE_CHECK([for fastcall calling convention], ac_cv_ffi_fastcall,
    [
      AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[#include <ffi.h>]],
        [[return FFI_FASTCALL]])
    ],
    [ac_cv_ffi_fastcall=yes], [ac_cv_ffi_fastcall=no])
  ])

  if test "$ac_cv_ffi_fastcall" = yes; then
    AC_DEFINE(HAVE_FFI_FASTCALL,1,[Whether libffi supports fastcall calling convention])
  fi

  AC_CACHE_CHECK([for thiscall calling convention], ac_cv_ffi_thiscall,
    [
      AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[#include <ffi.h>]],
        [[return FFI_THISCALL]])
    ],
    [ac_cv_ffi_thiscall=yes], [ac_cv_ffi_thiscall=no])
  ])

  if test "$ac_cv_ffi_thiscall" = yes; then
    AC_DEFINE(HAVE_FFI_THISCALL,1,[Whether libffi supports thiscall calling convention])
  fi

  AC_CACHE_CHECK([for stdcall calling convention], ac_cv_ffi_stdcall,
    [
      AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[#include <ffi.h>]],
        [[return FFI_STDCALL]])
    ],
    [ac_cv_ffi_stdcall=yes], [ac_cv_ffi_stdcall=no])
  ])

  if test "$ac_cv_ffi_stdcall" = yes; then
    AC_DEFINE(HAVE_FFI_STDCALL,1,[Whether libffi supports stdcall calling convention])
  fi

  AC_CACHE_CHECK([for pascal calling convention], ac_cv_ffi_pascal,
    [
      AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[#include <ffi.h>]],
        [[return FFI_PASCAL]])
    ],
    [ac_cv_ffi_pascal=yes], [ac_cv_ffi_pascal=no])
  ])

  if test "$ac_cv_ffi_pascal" = yes; then
    AC_DEFINE(HAVE_FFI_PASCAL,1,[Whether libffi supports pascal calling convention])
  fi

  AC_CACHE_CHECK([for register calling convention], ac_cv_ffi_register,
    [
      AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[#include <ffi.h>]],
        [[return FFI_REGISTER]])
    ],
    [ac_cv_ffi_register=yes], [ac_cv_ffi_register=no])
  ])

  if test "$ac_cv_ffi_register" = yes; then
    AC_DEFINE(HAVE_FFI_REGISTER,1,[Whether libffi supports register calling convention])
  fi

  AC_CACHE_CHECK([for ms_cdecl calling convention], ac_cv_ffi_ms_cdecl,
    [
      AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[#include <ffi.h>]],
        [[return FFI_MS_CDECL]])
    ],
    [ac_cv_ffi_ms_cdecl=yes], [ac_cv_ffi_ms_cdecl=no])
  ])

  if test "$ac_cv_ffi_ms_cdecl" = yes; then
    AC_DEFINE(HAVE_FFI_MS_CDECL,1,[Whether libffi supports ms_cdecl calling convention])
  fi

  AC_CACHE_CHECK([for sysv calling convention], ac_cv_ffi_sysv,
    [
      AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[#include <ffi.h>]],
        [[return FFI_SYSV]])
    ],
    [ac_cv_ffi_sysv=yes], [ac_cv_ffi_sysv=no])
  ])

  if test "$ac_cv_ffi_sysv" = yes; then
    AC_DEFINE(HAVE_FFI_SYSV,1,[Whether libffi supports sysv calling convention])
  fi

  PHP_NEW_EXTENSION(ffi, ffi.c ffi_parser.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(FFI_SHARED_LIBADD)
fi
