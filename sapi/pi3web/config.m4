PHP_ARG_WITH(pi3web,for Pi3Web Support,
[  --with-pi3web=DIR       Build PHP as a module for use with Pi3Web.])

if test "$PHP_PI3WEB" != "no"; then
	if test "$PHP_PI3WEB" = "yes"; then
		PI3PATH=../.. # the default
	else
		PI3PATH=$PHP_PI3WEB
	fi
	test -f "$PI3PATH/PiAPI/PiAPI.h" || AC_MSG_ERROR(Unable to find PiAPI.h in $PI3PATH/PiAPI)
	PHP_BUILD_THREAD_SAFE
	AC_DEFINE(WITH_PI3WEB,1,[whether you want Pi3Web support])
	AC_ADD_INCLUDE($PI3PATH/PiAPI)
	AC_ADD_INCLUDE($PI3PATH/Pi2API)
	AC_ADD_INCLUDE($PI3PATH/Pi3API)
	AC_ADD_INCLUDE($PI3PATH/PHP4)
	PHP_SAPI=pi3web
	PHP_BUILD_SHARED
	INSTALL_IT="\$(SHELL) \$(srcdir)/install-sh -m 0755 $SAPI_SHARED $PI3PATH/bin/"
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
