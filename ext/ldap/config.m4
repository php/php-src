dnl $Id$

PHP_ARG_WITH(ldap,whether to include LDAP support,
[  --with-ldap[=DIR]       Include LDAP support.  DIR is the LDAP base
                          install directory.])

if test "$PHP_LDAP" != "no"; then
	PHP_EXTENSION(ldap,$ext_shared)
	for i in /usr/local /usr $PHP_LDAP; do
		if test -f $i/include/ldap.h; then
			LDAP_DIR=$i
			LDAP_INCDIR=$i/include
			LDAP_LIBDIR=$i/lib
		elif test -f $i/include/umich-ldap/ldap.h; then
			LDAP_DIR=$i
			LDAP_INCDIR=$i/include/umich-ldap
			LDAP_LIBDIR=$i/lib
 		fi
	done

	if test -z "$LDAP_DIR"; then
		AC_MSG_ERROR(Cannot find ldap.h)
	fi

	dnl The Linux version of the SDK need -lpthread
	dnl I have tested Solaris, and it doesn't, but others may.  Add
	dnl these here if necessary.   -RL

	dnl Is this really necessary? -Troels Arvin

	if test `uname` = "Linux"; then
		LDAP_PTHREAD="pthread"
	else
		LDAP_PTHREAD=
	fi

	PHP_SUBST(LDAP_SHARED_LIBADD)

	if test -f $LDAP_LIBDIR/liblber.a; then
		AC_ADD_LIBRARY_WITH_PATH(lber, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
		AC_ADD_LIBRARY_WITH_PATH(ldap, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
	elif test -f $LDAP_LIBDIR/libldapssl30.so; then
		AC_ADD_LIBRARY($LDAP_PTHREAD)
		AC_ADD_LIBRARY_WITH_PATH(ldapssl30, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
		AC_DEFINE(HAVE_NSLDAP,1,[ ])
	elif test -f $LDAP_LIBDIR/libldapssl30.sl; then
		AC_ADD_LIBRARY_WITH_PATH(ldapssl30, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
		AC_DEFINE(HAVE_NSLDAP,1,[ ])
	elif test -f $LDAP_LIBDIR/libldap30.so; then
		AC_ADD_LIBRARY($LDAP_PTHREAD)
		AC_ADD_LIBRARY_WITH_PATH(ldap30, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
		AC_DEFINE(HAVE_NSLDAP,1,[ ])
	elif test -f $LDAP_LIBDIR/libldap30.sl; then
		AC_ADD_LIBRARY_WITH_PATH(ldap30, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
		AC_DEFINE(HAVE_NSLDAP,1,[ ])
	elif test -f $LDAP_LIBDIR/libumich_ldap.so; then
		AC_ADD_LIBRARY_WITH_PATH(umich_lber, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
		AC_ADD_LIBRARY_WITH_PATH(umich_ldap, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
	fi  

	AC_ADD_INCLUDE($LDAP_INCDIR)
	AC_DEFINE(HAVE_LDAP,1,[ ])

fi
