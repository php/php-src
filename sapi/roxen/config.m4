dnl ## $Id$ -*- sh -*-

RESULT=no
AC_MSG_CHECKING(for Roxen/Pike support)
AC_ARG_WITH(roxen,
[  --with-roxen=DIR],
[
	if test ! -d $withval ; then
		AC_MSG_ERROR(You did not specify a directory)
	fi
/*	enable_thread_safety=yes*/
/*	passthru="$passthru --enable-thread-safety"*/
	ROXEN_DIR=$withval
	AC_ADD_INCLUDE($ROXEN_DIR/pike/0.7.79/include/pike/)
	AC_DEFINE(HAVE_ROXEN)
	PHP_SAPI=roxen
	PHP_BUILD_SHARED
	INSTALL_IT="\$(SHELL) \$(srcdir)/install-sh -m 0755 $SAPI_SHARED $ROXEN_DIR/pike/0.7.79/lib/modules/PHP4.so"
	RESULT=yes
])
AC_MSG_RESULT($RESULT)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
	