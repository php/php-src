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
		elif test -f /usr/include/umich-ldap/ldap.h; then
			LDAP_INCDIR=/usr/include/umich-ldap
			LDAP_LIBDIR=/usr/lib
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
		LDAP_PTHREAD="pthread"
	else
		LDAP_PTHREAD=
	fi

	if test -f $LDAP_LIBDIR/liblber.a; then
		LDAP_LIBS="-lldap -llber"
        AC_ADD_LIBRARY(ldap)
        AC_ADD_LIBRARY(lber)
	elif test -f $LDAP_LIBDIR/libldapssl30.so; then
        AC_ADD_LIBRARY(ldapssl30)
        AC_ADD_LIBRARY($LDAP_PTHREAD)
		AC_DEFINE(HAVE_NSLDAP,1,[ ])
	elif test -f $LDAP_LIBDIR/libldapssl30.sl; then
        AC_ADD_LIBRARY(ldapssl30)
		AC_DEFINE(HAVE_NSLDAP,1,[ ])
	elif test -f $LDAP_LIBDIR/libldap30.so; then
        AC_ADD_LIBRARY(ldap30)
        AC_ADD_LIBRARY($LDAP_PTHREAD)
		AC_DEFINE(HAVE_NSLDAP,1,[ ])
	elif test -f $LDAP_LIBDIR/libldap30.sl; then
        AC_ADD_LIBRARY(ldap30)
		AC_DEFINE(HAVE_NSLDAP,1,[ ])
	elif test -f $LDAP_LIBDIR/libumich_ldap.so; then
        AC_ADD_LIBRARY(umich_ldap)
        AC_ADD_LIBRARY(umich_lber)
	fi  

    AC_ADD_INCLUDE($LDAP_INCDIR)
    AC_ADD_LIBPATH($LDAP_LIBDIR)

    AC_DEFINE(HAVE_LDAP,1,[ ])

    AC_MSG_RESULT(yes)
    PHP_EXTENSION(ldap)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
