dnl $Id$

PHP_ARG_WITH(bcmath, for bc style precision math functions,
[  --disable-bcmath         Compile without bc style precision math functions. ], yes)

if test "$PHP_BCMATH" != "no"; then
  AC_DEFINE(WITH_BCMATH, 1, [Whether you have bcmath])
  PHP_EXTENSION(bcmath, $ext_shared)
  PHP_FAST_OUTPUT($ext_builddir/libbcmath/Makefile $ext_builddir/libbcmath/src/Makefile)
  LIB_BUILD($ext_builddir/libbcmath,$ext_shared,yes)
  LIB_BUILD($ext_builddir/libbcmath/src,$ext_shared,yes)
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
