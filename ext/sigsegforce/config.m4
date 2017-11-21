PHP_ARG_ENABLE(sigsegforce, enable sigsegforce function to reliably create a segfault
[ --enable-sigsegforce   Enable force of SIGFAULT support])
if test "$PHP_SIGSEGFORCE" = "yes"; then
  AC_DEFINE(PHP_SIGSEGFORCE, 1, [Whether you have SIGFAULT FORCE])
  PHP_NEW_EXTENSION(sigsegforce, sigsegforce.c, $ext_shared)
fi
