dnl ## $Id$ -*- sh -*-

RESULT=no
AC_MSG_CHECKING(for AOLserver support)
AC_ARG_WITH(aolserver,
[  --with-aolserver=DIR],
[
	if test ! -d $withval ; then
		AC_MSG_ERROR(You did not specify a directory)
	fi
	if test "$enable_thread_safety" != "yes"; then
		AC_MSG_ERROR(AOLserver must be compiled using --enable-thread-safety)
	fi
	NS_DIR=$withval
	AC_ADD_INCLUDE($NS_DIR/include)
	AC_DEFINE(HAVE_AOLSERVER)
	PHP_SAPI=nsapi
	SAPI_TARGET=php4_nsapi.so
	INSTALL_IT="\$(SHELL) \$(srcdir)/install-sh -m 0755 $SAPI_TARGET $NS_DIR/root/bin/$SAPI_TARGET"
	RESULT=yes
])
AC_MSG_RESULT($RESULT)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
