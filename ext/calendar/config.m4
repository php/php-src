dnl $Id$

AC_MSG_CHECKING(whether to enable calendar conversion support)
AC_ARG_ENABLE(calendar,
[  --enable-calendar       Enable support for calendar conversion],
  if test "$withval" = "yes"; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_CALENDAR,1,[ ])
    PHP_EXTENSION(calendar)
  fi
[
],[
  AC_MSG_RESULT(no)
])
