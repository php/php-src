dnl $Id$

AC_MSG_CHECKING(for ICAP support)
AC_ARG_WITH(icap,
[  --with-icap[=DIR]       Include ICAP support (DIR is the ICAP include dir and lib dir).],
[
	if test "$withval" = "yes"; then
		if test -f /usr/local/include/icap.h; then
		withval=/usr/local
		elif test -f /usr/include/icap.h; then
			withval=/usr
		fi
	fi
	if test "$withval" != "no" && test "$withval" != "yes"; then
		ICAP_DIR=$withval
		if test -f $ICAP_DIR/include/icap/icap.h; then
			ICAP_INC_DIR=$ICAP_DIR/include/icap
		else
			ICAP_INC_DIR=$withval/include
		fi
		ICAP_LIBDIR="$ICAP_DIR/lib"
		ICAP_LIB="c-client"
		AC_ADD_LIBRARY_WITH_PATH($ICAP_LIB, $ICAP_LIBDIR)
		AC_ADD_INCLUDE($ICAP_INC_DIR)
		AC_DEFINE(HAVE_ICAP)
		AC_MSG_RESULT(yes)
		PHP_EXTENSION(icap)
	else
		AC_MSG_ERROR(could not find icap library!)
	fi
],[
    AC_MSG_RESULT(no)
])
  

