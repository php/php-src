dnl $Id$
dnl config.m4 for extension session
dnl don't forget to call PHP_EXTENSION(session)


MM_RESULT=no
AC_MSG_CHECKING(for mm support)
AC_ARG_WITH(mm,
[  --with-mm[=DIR]         Include mm support],
[
	if test "$withval" != "no"; then
		for i in /usr/local /usr $withval; do
			if test -f "$i/include/mm.h"; then
			  MM_DIR="$i"
			fi
		done
		
		if test -z "$MM_DIR" ; then
			AC_MSG_ERROR(cannot find mm library)
		fi

		AC_ADD_LIBRARY_WITH_PATH(mm, $MM_DIR/lib)
		AC_ADD_INCLUDE($MM_DIR/include)
		AC_DEFINE(HAVE_LIBMM, 1)
		MM_RESULT=yes
		PHP_EXTENSION(ps_mm)
	fi
])
AC_MSG_RESULT($MM_RESULT)

PHP_EXTENSION(session)
