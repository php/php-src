dnl $Id$
dnl config.m4 for extension yp
dnl don't forget to call PHP_EXTENSION(yp)

AC_MSG_CHECKING(whether to include YP support)
AC_ARG_WITH(yp,
[  --with-yp               Include YP support],
[
	if test "$withval" = "yes"; then
		AC_MSG_RESULT(yes)
		AC_DEFINE(HAVE_YP,,[ ])
		PHP_EXTENSION(yp)
		if test `uname` = "SunOS";then
			release=`uname -r`
			case "$release" in
				5*)
					AC_DEFINE(SOLARIS_YP,,[ ])
					;;
				*)
					;;
			esac
		fi
	else
		AC_MSG_RESULT(no)
	fi
],[
	AC_MSG_RESULT(no)
])

