PHP_ARG_ENABLE([posix],
  [whether to enable POSIX-like functions],
  [AS_HELP_STRING([--disable-posix],
    [Disable POSIX-like functions])],
  [yes])

if test "$PHP_POSIX" = "yes"; then
  AC_DEFINE(HAVE_POSIX, 1, [whether to include POSIX-like functions])
  PHP_NEW_EXTENSION(posix, posix.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)

  AC_CHECK_FUNCS(seteuid setegid setsid getsid getpgid ctermid mkfifo mknod setrlimit getrlimit getgroups initgroups getgrgid_r eaccess mkfifoat mknodat)

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
  AS_IF([command -v ldd >/dev/null && ldd --version 2>&1 | grep -q "^musl"],[],
    [AC_CHECK_FUNCS(pathconf fpathconf)])

  AC_MSG_CHECKING([for working ttyname_r() implementation])
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <unistd.h>

int main(int argc, char *argv[])
{
	char buf[64];

	return !ttyname_r(0, buf, 64);
}
  ]])],[
    AC_MSG_RESULT([yes])
    AC_DEFINE(HAVE_TTYNAME_R, 1, [Whether you have a working ttyname_r])
  ],[
    AC_MSG_RESULT([no, posix_ttyname() will be thread-unsafe])
  ], [
    AC_MSG_RESULT([no, cannot detect working ttyname_r() when cross compiling. posix_ttyname() will be thread-unsafe])
  ])

  AC_CHECK_MEMBERS([struct utsname.domainname],,,[
    #ifndef _GNU_SOURCE
    #define _GNU_SOURCE
    #endif
    #include <sys/utsname.h>
  ])
fi
