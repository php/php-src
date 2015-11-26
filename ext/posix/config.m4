dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(posix,whether to enable POSIX-like functions,
[  --disable-posix         Disable POSIX-like functions], yes)

if test "$PHP_POSIX" = "yes"; then
  AC_DEFINE(HAVE_POSIX, 1, [whether to include POSIX-like functions])
  PHP_NEW_EXTENSION(posix, posix.c, $ext_shared)

  AC_CHECK_HEADERS(sys/mkdev.h)

  AC_CHECK_FUNCS(seteuid setegid setsid getsid setpgid getpgid ctermid mkfifo mknod setrlimit getrlimit getlogin getgroups makedev initgroups getpwuid_r getgrgid_r)

  AC_MSG_CHECKING([for working ttyname_r() implementation])
  AC_TRY_RUN([
#include <unistd.h>

int main(int argc, char *argv[])
{
	char buf[64];

	return ttyname_r(0, buf, 64) ? 1 : 0;
}
  ],[
    AC_MSG_RESULT([yes])
    AC_DEFINE(HAVE_TTYNAME_R, 1, [Whether you have a working ttyname_r])
  ],[
    AC_MSG_RESULT([no, posix_ttyname() will be thread-unsafe])
  ], [
    AC_MSG_RESULT([no, cannot detect working ttyname_r() when cross compiling. posix_ttyname() will be thread-unsafe])
  ])

  AC_CACHE_CHECK([for utsname.domainname], ac_cv_have_utsname_domainname, [
    AC_TRY_COMPILE([
      #define _GNU_SOURCE
      #include <sys/utsname.h>
    ],[
      return sizeof(((struct utsname *)0)->domainname);
    ],[
      ac_cv_have_utsname_domainname=yes
    ],[
      ac_cv_have_utsname_domainname=no
    ])
  ])
  if test "$ac_cv_have_utsname_domainname" = yes; then
    AC_DEFINE(HAVE_UTSNAME_DOMAINNAME, 1, [Whether struct utsname has domainname])
  fi
fi
