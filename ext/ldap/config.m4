dnl $Id$

AC_MSG_CHECKING(for LDAP support)
AC_ARG_WITH(ldap,
[  --with-ldap[=DIR]       Include LDAP support.  DIR is the LDAP base
                          install directory, defaults to /usr/local/ldap],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
		if test -f /usr/include/ldap.h; then 
			LDAP_INCDIR=/usr/include
			LDAP_LIBDIR=/usr/lib
		elif test -f /usr/local/include/ldap.h; then
			LDAP_INCDIR=/usr/local/include
			LDAP_LIBDIR=/usr/local/lib
		elif test -f /usr/local/ldap/include/ldap.h; then
			LDAP_INCDIR=/usr/local/ldap/include
			LDAP_LIBDIR=/usr/local/ldap/lib
		else
			AC_MSG_RESULT(no)
			AC_MSG_ERROR(Unable to find ldap.h)
		fi	
    else
		if test -f $withval/include/ldap.h; then
			LDAP_INCDIR=$withval/include
			LDAP_LIBDIR=$withval/lib
		else
			AC_MSG_RESULT(no)
			AC_MSG_ERROR(Unable to find $withval/include/ldap.h)
		fi
    fi

dnl The Linux version of the SDK need -lpthread
dnl I have tested Solaris, and it doesn't, but others may.  Add
dnl these here if necessary.   -RL

	if test `uname` = "Linux"; then
		LDAP_PTHREAD="-lpthread"
	else
		LDAP_PTHREAD=
	fi

	if test -f $LDAP_LIBDIR/liblber.a; then
		LDAP_LIBS="-lldap -llber"
	elif test -f $LDAP_LIBDIR/libldapssl30.so; then
		LDAP_LIBS="-lldapssl30 $LDAP_PTHREAD"
		AC_DEFINE(HAVE_NSLDAP)
	elif test -f $LDAP_LIBDIR/libldapssl30.sl; then
		LDAP_LIBS="-lldapssl30"
		AC_DEFINE(HAVE_NSLDAP)
	elif test -f $LDAP_LIBDIR/libldap30.so; then
		LDAP_LIBS="-lldap30 $LDAP_PTHREAD"
		AC_DEFINE(HAVE_NSLDAP)
	elif test -f $LDAP_LIBDIR/libldap30.sl; then
		LDAP_LIBS="-lldap30"
		AC_DEFINE(HAVE_NSLDAP)
	fi  

	LDAP_INCLUDE=-I$LDAP_INCDIR
	if test -n "$APXS"; then
	    LDAP_LFLAGS="-L$LDAP_LIBDIR ${apxs_runpath_switch}$LDAP_LIBDIR'"
	else
	    LDAP_LFLAGS="-L$LDAP_LIBDIR ${ld_runpath_switch}$LDAP_LIBDIR"
	fi

	INCLUDES="$INCLUDES $LDAP_INCLUDE"
	EXTRA_LIBS="$EXTRA_LIBS $LDAP_LFLAGS $LDAP_LIBS"

    AC_DEFINE(HAVE_LDAP)

    AC_MSG_RESULT(yes)
    PHP_EXTENSION(ldap)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
