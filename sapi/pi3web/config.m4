dnl
dnl $Id$
dnl

RESULT=no
AC_MSG_CHECKING(for Pi3Web support)

AC_ARG_WITH(pi3web,
[  --with-pi3web=DIR       Build PHP as Pi3Web module],
[
if test "$withval" != "no"; then
	if test "$withval" = "yes"; then
		PI3PATH=../.. # the default
	else
		PI3PATH=$withval
	fi
	test -f "$PI3PATH/PiAPI/PiAPI.h" || AC_MSG_ERROR(Unable to find PiAPI.h in $PI3PATH/PiAPI)
	PHP_BUILD_THREAD_SAFE
	AC_DEFINE(WITH_PI3WEB,1,[whether you want Pi3Web support])
	PHP_ADD_INCLUDE($PI3PATH/PiAPI)
	PHP_ADD_INCLUDE($PI3PATH/Pi2API)
	PHP_ADD_INCLUDE($PI3PATH/Pi3API)
	PHP_ADD_INCLUDE($PI3PATH/PHP4)
	PHP_SELECT_SAPI(pi3web, shared, pi3web_sapi.c)
	INSTALL_IT="\$(SHELL) \$(srcdir)/install-sh -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$PI3PATH/bin/"
	RESULT=yes
else
	RESULT=no
fi
])
AC_MSG_RESULT($RESULT)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
