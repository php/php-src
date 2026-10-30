PHP_ARG_ENABLE([posix],
  [whether to enable POSIX-like functions],
  [AS_HELP_STRING([--disable-posix],
    [Disable POSIX-like functions])],
  [yes])

if test "$PHP_POSIX" = "yes"; then
  AC_DEFINE([HAVE_POSIX], [1],
    [Define to 1 if the PHP extension 'posix' is available.])
  PHP_NEW_EXTENSION([posix],
    [posix.c],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])

  AC_CHECK_FUNCS(m4_normalize([
    ctermid
    eaccess
    getgrgid_r
    getgroups
    getpgid
    getrlimit
    getsid
    initgroups
    mkfifo
    mknod
    setegid
    seteuid
    setrlimit
    setsid
  ]))

  dnl Check for makedev. If it's defined as a macro, AC_CHECK_FUNCS won't work.
  dnl Required headers are included by the AC_HEADER_MAJOR logic.
  AC_CHECK_FUNCS([makedev],,
    [AC_CHECK_DECL([makedev], [AC_DEFINE([HAVE_MAKEDEV], [1])],, [
      #include <sys/types.h>
      #ifdef MAJOR_IN_MKDEV
      # include <sys/mkdev.h>
      #elif defined(MAJOR_IN_SYSMACROS)
      # include <sys/sysmacros.h>
      #endif
    ])])

dnl Skip pathconf and fpathconf check on musl libc due to limited implementation
dnl (first argument is not validated and has different error).
  AS_IF([command -v ldd >/dev/null && ldd --version 2>&1 | grep ^musl >/dev/null 2>&1],
    [],
    [AC_CHECK_FUNCS([pathconf fpathconf])])

  AC_CACHE_CHECK([for working ttyname_r() implementation],
    [php_cv_func_ttyname_r],
    [AC_RUN_IFELSE([AC_LANG_PROGRAM([#include <unistd.h>], [[
      char buf[64];
      return !ttyname_r(0, buf, 64);
    ]])],
    [php_cv_func_ttyname_r=yes],
    [php_cv_func_ttyname_r=no],
    [AC_CHECK_FUNC([ttyname_r],
      [php_cv_func_ttyname_r=yes], [php_cv_func_ttyname_r=no])])])
  AS_VAR_IF([php_cv_func_ttyname_r], [yes],
    [AC_DEFINE([HAVE_TTYNAME_R], [1],
      [Define to 1 if you have a working 'ttyname_r' function.])],
    [AC_MSG_NOTICE([posix_ttyname() will be thread-unsafe])])

  AC_CHECK_MEMBERS([struct utsname.domainname],,,[
    #ifndef _GNU_SOURCE
    #define _GNU_SOURCE
    #endif
    #include <sys/utsname.h>
  ])
fi
