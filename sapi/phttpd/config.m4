dnl
dnl $Id$
dnl 

RESULT=no
AC_MSG_CHECKING(for PHTTPD support)
AC_ARG_WITH(phttpd,
[  --with-phttpd=DIR       Build PHP as phttpd module],
[
	if test ! -d $withval ; then
		AC_MSG_ERROR(You did not specify a directory)
	fi
	PHP_BUILD_THREAD_SAFE
	PHTTPD_DIR=$withval
	PHP_ADD_INCLUDE($PHTTPD_DIR/include)
	AC_DEFINE(HAVE_PHTTPD,1,[Whether you have phttpd])
	PHP_SAPI=phttpd
	PHP_BUILD_SHARED
	INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$PHTTPD_DIR/modules/"
	RESULT=yes
])
AC_MSG_RESULT($RESULT)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
