dnl $Id$
dnl config.m4 for extension posix
dnl don't forget to call PHP_EXTENSION(posix)

PHP_ARG_ENABLE(posix,whether to include POSIX-like functions,
[  --disable-posix         Disable POSIX-like functions], yes)

if test "$PHP_POSIX" = "yes"; then
  AC_DEFINE(HAVE_POSIX, 1, [whether to include POSIX-like functions])
  PHP_EXTENSION(posix, $ext_shared)

  AC_CHECK_FUNCS(seteuid setegid setsid getsid setpgid getpgid ctermid mkfifo getrlimit)
fi
