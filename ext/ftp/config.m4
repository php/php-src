dnl $Id$
dnl config.m4 for extension ftp
dnl don't forget to call PHP_EXTENSION(ftp)

AC_MSG_CHECKING(for FTP support)
AC_ARG_WITH(ftp,
[  --with-ftp              Include FTP support.],
[
  if test "$withval" != "no"; then
    AC_DEFINE(HAVE_FTP)
    PHP_EXTENSION(ftp)
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
