dnl ## $Id$ -*- sh -*-

RESULT=no
AC_MSG_CHECKING(for AOLserver support)
AC_ARG_WITH(aolserver,
[  --with-aolserver=DIR],
[
	if test ! -d $withval ; then
		AC_MSG_ERROR(You did not specify a directory)
	fi
	enable_thread_safety=yes
	passthru="$passthru --enable-thread-safety"
	NS_DIR=$withval
	AC_ADD_INCLUDE($NS_DIR/include)
	AC_DEFINE(HAVE_AOLSERVER)
	PHP_SAPI=nsapi
	PHP_BUILD_SHARED
	INSTALL_IT="\$(SHELL) \$(srcdir)/install-sh -m 0755 libs/libphp4.so $NS_DIR/root/bin/libphp4.so"
	RESULT=yes
])
AC_MSG_RESULT($RESULT)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
