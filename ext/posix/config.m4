dnl $Id$
dnl config.m4 for extension posix
dnl don't forget to call PHP_EXTENSION(posix)

AC_ARG_ENABLE(posix,
[  --disable-posix         Disable POSIX-like functions],[
  PHP_POSIX=$enableval
],[
  PHP_POSIX=yes
])

AC_MSG_CHECKING(whether to include POSIX-like functions)
AC_MSG_RESULT($PHP_POSIX)

if test "$PHP_POSIX" = "yes"; then
  AC_DEFINE(HAVE_POSIX, 1, [whether to include POSIX-like functions])
  PHP_EXTENSION(posix)
fi
