dnl ## $Id$ -*- sh -*-

dnl AC_MSG_CHECKING(for Zeus ISAPI support)
dnl AC_ARG_WITH(zeus,
dnl [  --with-zeus=DIR         Build PHP as an ISAPI module for use with Zeus.],
dnl [
dnl 	if test "$withval" != "no"; then
dnl 		AC_MSG_RESULT(yes)
dnl 		if test "${enable_thread_safety}" != "yes"; then
dnl 			AC_MSG_ERROR(You must specify --enable-thread-safety to build as an ISAPI module)
dnl 		fi
dnl 		if test "$withval" = "yes"; then
dnl 			zeuspath=/usr/local/zeus # the default
dnl 		fi
dnl 		if ! test -f "$zeuspath/web/include/httpext.h"; then
dnl 			AC_MSG_ERROR(Unable to find httpext.h in $zeuspath/web/include)
dnl 		fi
dnl 		AC_DEFINE(WITH_ZEUS)
dnl 		AC_ADD_INCLUDE($zeuspath/web/include)
dnl 		PHP_SAPI=isapi
dnl 		SAPI_TARGET=php4isapi.so
dnl 	else
dnl 		AC_MSG_RESULT(no)
dnl 	fi
dnl ],[
dnl 	AC_MSG_RESULT(no)
dnl ])

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
