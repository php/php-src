dnl ## $Id$ -*- sh -*-

AC_MSG_CHECKING(for NSAPI support)
AC_ARG_WITH(nsapi,
[  --with-nsapi=DIR             Specify path to the installed Netscape],[
  PHP_NSAPI=$withval
],[
  PHP_NSAPI=no
])
AC_MSG_RESULT($PHP_NSAPI)

if test "$PHP_NSAPI" != "no"; then
  if test ! -d $PHP_NSAPI/bin ; then
    AC_MSG_ERROR(Please specify the path to the root of your Netscape server using --with-nsapi=DIR)
  fi
  PHP_BUILD_THREAD_SAFE
  AC_ADD_INCLUDE($PHP_NSAPI/include)
  AC_DEFINE(HAVE_NSAPI,1,[Whether you have a Netscape Server])
  PHP_SAPI=nsapi
  PHP_BUILD_SHARED
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED $PHP_NSAPI/bin/"
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
