dnl ## $Id$ -*- sh -*-

AC_MSG_CHECKING(for Continuity support)
AC_ARG_WITH(continuity,
[  --with-continuity=DIR   Build PHP as Continuity Server module. 
                          DIR is path to the installed Continuity Server root],[
  PHP_CONTINUITY=$withval
],[
  PHP_CONTINUITY=no
])
AC_MSG_RESULT($PHP_CONTINUITY)

if test "$PHP_CONTINUITY" != "no"; then

  if test ! -d $PHP_CONTINUITY; then
    AC_MSG_ERROR(Please specify the path to the root of your Continuity server using --with-continuity=DIR)
  fi
  AC_MSG_CHECKING(for Continuity include files)
  if test -d $PHP_CONTINUITY/include ; then
    CAPI_INCLUDE=$PHP_CONTINUITY/include
    AC_MSG_RESULT(Continuity Binary Distribution)
  else
    AC_MSG_ERROR(Cannot find your CAPI include files in either DIR/src or DIR/include)
  fi

  PHP_SELECT_SAPI(continuity,shared,capi.c)
  PHP_ADD_INCLUDE($CAPI_INCLUDE)
  PHP_BUILD_THREAD_SAFE
  AC_DEFINE(HAVE_CONTINUITY,1,[Whether you have a Continuity Server])
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$PHP_CONTINUITY/lib/"
fi


dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
