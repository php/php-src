dnl $Id$
dnl config.m4 for extension session
dnl don't forget to call PHP_EXTENSION(session)

RESULT=no
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
		RESULT=yes
		PHP_EXTENSION(ps_mm)
	fi
])
AC_MSG_RESULT($RESULT)

RESULT=no
AC_MSG_CHECKING(whether to enable transparent session id propagation)
AC_ARG_ENABLE(trans-sid,
[  --enable-trans-sid      Enable transparent session id propagation],
[
  if test "$enableval" = "yes" ; then
    AC_DEFINE(TRANS_SID, 1)
	RESULT=yes
  fi
])
AC_MSG_RESULT($RESULT)

PHP_EXTENSION(session)
