dnl ## $Id$ -*- sh -*-

AC_MSG_CHECKING(for Zeus ISAPI support)
AC_ARG_WITH(zeus,
[  --with-zeus=DIR         Build PHP as an ISAPI module for use with Zeus.],
[
	if test "$withval" != "no"; then
		SAPI_TARGET=libphp4_isapi.so
		AC_MSG_RESULT(yes)
		if test "${enable_thread_safety}" != "yes"; then
			AC_MSG_ERROR(You must specify --enable-thread-safety to build as an ISAPI module)
		fi
		if test "$withval" = "yes"; then
			zeuspath=/usr/local/zeus # the default
		fi
		if ! test -f "$zeuspath/web/include/httpext.h"; then
			AC_MSG_ERROR(Unable to find httpext.h in $zeuspath/web/include)
		fi
		AC_ADD_INCLUDE($zeuspath/web/include)
		PHP_SAPI=isapi
		SAPI_TARGET=php4isapi.so
	else
		AC_MSG_RESULT(no)
	fi
],[
	AC_MSG_RESULT(no)
])

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
