dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(bcmath, whether to enable bc style precision math functions,
[  --enable-bcmath         Enable bc style precision math functions.])

if test "$PHP_BCMATH" != "no"; then
  AC_DEFINE(WITH_BCMATH, 1, [Whether you have bcmath])
  PHP_EXTENSION(bcmath, $ext_shared)
  PHP_FAST_OUTPUT($ext_builddir/libbcmath/Makefile $ext_builddir/libbcmath/src/Makefile)
  LIB_BUILD($ext_builddir/libbcmath,$ext_shared,yes)
  LIB_BUILD($ext_builddir/libbcmath/src,$ext_shared,yes)
fi
