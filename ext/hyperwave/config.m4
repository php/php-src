dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for Hyperwave support)
AC_ARG_WITH(hyperwave,
[  --with-hyperwave        Include Hyperwave support],
[
  if test "$withval" != "no"; then
    AC_DEFINE(HYPERWAVE,1,[ ])
    AC_MSG_RESULT(yes)
    PHP_NEW_EXTENSION(hyperwave, hw.c hg_comm.c)
  else
    AC_DEFINE(HYPERWAVE,0,[ ])
    AC_MSG_RESULT(no)
  fi
],[
  AC_DEFINE(HYPERWAVE,0,[ ])
  AC_MSG_RESULT(no)
])
