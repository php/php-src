dnl $Id$

AC_MSG_CHECKING(for ICAP support)
AC_ARG_WITH(icap,
[  --with-icap[=DIR]     Include ICAP support.],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      ICAP_DIR=/usr/local
    else
      ICAP_DIR=$withval
    fi
    
    AC_ADD_INCLUDE($ICAP_DIR/)
    AC_ADD_LIBRARY_WITH_PATH(icap, $ICAP_DIR/)
    AC_DEFINE(HAVE_ICAP)
    AC_MSG_RESULT(yes)
  else
    AC_MSG_ERROR(no)
  fi
],[
  AC_MSG_RESULT(no)
])

