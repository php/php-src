dnl $Id$

PHP_ARG_ENABLE(bcmath,whether to enable bc style precision math functions,
[  --enable-bcmath         Compile with bc style precision math functions.
                          Read README-BCMATH for instructions on how to
                          get this module installed. ])

if test "$PHP_BCMATH" != "no"; then
  AC_DEFINE(WITH_BCMATH, 1, [Whether you have bcmath])
  PHP_EXTENSION(bcmath, $ext_shared)
  if test ! -f $ext_srcdir/number.c || grep "Dummy File" $ext_srcdir/number.c >/dev/null; then
    AC_MSG_ERROR(You do not have the bcmath package. Please read the README.BCMATH file.)
  fi
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
