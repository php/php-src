dnl $Id$
dnl config.m4 for extension wddx

AC_MSG_CHECKING(whether to include WDDX support)
AC_ARG_WITH(wddx,
[  --with-wddx             Include WDDX support],[
  if test "$withval" = "yes"; then
    if test "${with_xml+set}" != "set" -o "$with_xml" = "no"; then
	AC_MSG_ERROR(WDDX requires --with-xml)
    else
        AC_DEFINE(HAVE_WDDX, 1)
        AC_MSG_RESULT(yes)
        PHP_EXTENSION(wddx)
    fi
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
]) 
