dnl Process this file with autoconf to produce a configure script.
AC_INIT(doc/bcmath.1)
AM_INIT_AUTOMAKE("bcmath", "0.2")
AM_CONFIG_HEADER(config.h)

AC_PROG_CC

AC_PROG_INSTALL
AC_PROG_RANLIB
AC_PROG_MAKE_SET

AC_CHECK_HEADERS(stdarg.h stddef.h stdlib.h string.h limits.h unistd.h lib.h)
AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_TYPE(ptrdiff_t, size_t)

AC_OUTPUT(Makefile src/Makefile doc/Makefile)

