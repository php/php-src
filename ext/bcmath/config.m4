dnl $Id$

AC_MSG_CHECKING(whether to enable bc style precision math functions)
AC_ARG_ENABLE(bcmath,
[  --enable-bcmath         Compile with bc style precision math functions.
                          Read README-BCMATH for instructions on how to
                          get this module installed. ],
[
  if test "$enableval" = "yes"; then
    AC_DEFINE(WITH_BCMATH, 1, [Whether you have bcmath])
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(bcmath)
	if test -f $srcdir/ext/bcmath/number.c; then
		if grep "Dummy File" $srcdir/ext/bcmath/number.c >/dev/null; then
		  AC_MSG_ERROR(You do not have the bcmath package. Please read the README-BCMATH.)
		fi
	else
		AC_MSG_ERROR(You do not have the bcmath package. Please read the README-BCMATH.)
	fi
  else
    AC_DEFINE(WITH_BCMATH, 0,[ ])
    AC_MSG_RESULT(no)
  fi
],[
  AC_DEFINE(WITH_BCMATH, 0,[ ])
  AC_MSG_RESULT(no)
]) 

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
