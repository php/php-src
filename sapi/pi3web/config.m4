RESULT=no
AC_MSG_CHECKING(for Pi3Web Support)
AC_ARG_WITH(pi3web,
[  --with-pi3web=DIR         Build PHP as an module for use with Pi3Web.],
[
	if test "$withval" = "yes"; then
		PI3PATH=../.. # the default
	else
		PI3PATH=$withval
	fi
	pwd
	test -f "$PI3PATH/PiAPI/PiAPI.h" || AC_MSG_ERROR(Unable to find PiAPI.h in $PI3PATH/PiAPI)
	PHP_BUILD_THREAD_SAFE
	AC_DEFINE(WITH_PI3WEB,1,[ ])
	AC_ADD_INCLUDE($PI3PATH/PiAPI)
	AC_ADD_INCLUDE($PI3PATH/Pi2API)
	AC_ADD_INCLUDE($PI3PATH/Pi3API)
	AC_ADD_INCLUDE($PI3PATH/PHP4)
	PHP_SAPI=pi3web
	PHP_BUILD_SHARED
	INSTALL_IT="\$(SHELL) \$(srcdir)/install-sh -m 0755 $SAPI_SHARED $PI3PATH/bin/"
	RESULT=yes
])
AC_MSG_RESULT($RESULT)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
