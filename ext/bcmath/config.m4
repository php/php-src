PHP_ARG_ENABLE([bcmath],
  [whether to enable bc style precision math functions],
  [AS_HELP_STRING([--enable-bcmath],
    [Enable bc style precision math functions])])

if test "$PHP_BCMATH" != "no"; then
  PHP_NEW_EXTENSION([bcmath], m4_normalize([
      bcmath.c
      libbcmath/src/add.c
      libbcmath/src/compare.c
      libbcmath/src/convert.c
      libbcmath/src/div.c
      libbcmath/src/divmod.c
      libbcmath/src/doaddsub.c
      libbcmath/src/floor_or_ceil.c
      libbcmath/src/long2num.c
      libbcmath/src/init.c
      libbcmath/src/int2num.c
      libbcmath/src/nearzero.c
      libbcmath/src/neg.c
      libbcmath/src/num2long.c
      libbcmath/src/num2str.c
      libbcmath/src/raise.c
      libbcmath/src/raisemod.c
      libbcmath/src/recmul.c
      libbcmath/src/rmzero.c
      libbcmath/src/round.c
      libbcmath/src/sqrt.c
      libbcmath/src/str2num.c
      libbcmath/src/sub.c
      libbcmath/src/zero.c
    ]),
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
  PHP_ADD_BUILD_DIR([$ext_builddir/libbcmath/src])
  AC_DEFINE([HAVE_BCMATH], [1],
    [Define to 1 if the PHP extension 'bcmath' is available.])
fi
