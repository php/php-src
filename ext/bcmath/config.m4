dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(bcmath, whether to enable bc style precision math functions,
[  --enable-bcmath         Enable bc style precision math functions.])

if test "$PHP_BCMATH" != "no"; then
  AC_DEFINE(WITH_BCMATH, 1, [Whether you have bcmath])
  PHP_NEW_EXTENSION(bcmath, bcmath.c \
libbcmath/src/add.c libbcmath/src/div.c libbcmath/src/init.c libbcmath/src/neg.c libbcmath/src/outofmem.c libbcmath/src/raisemod.c libbcmath/src/rt.c libbcmath/src/sub.c \
libbcmath/src/compare.c libbcmath/src/divmod.c libbcmath/src/int2num.c libbcmath/src/num2long.c libbcmath/src/output.c libbcmath/src/recmul.c \
libbcmath/src/sqrt.c libbcmath/src/zero.c libbcmath/src/debug.c libbcmath/src/doaddsub.c libbcmath/src/nearzero.c libbcmath/src/num2str.c libbcmath/src/raise.c \
libbcmath/src/rmzero.c libbcmath/src/str2num.c,
          $ext_shared,,-I@ext_srcdir@/libbcmath/src)
  PHP_ADD_BUILD_DIR($ext_builddir/libbcmath/src)
fi
