dnl
dnl $Id$
dnl

PHP_ARG_WITH(vpopmail, for vpopmail support,
[  --with-vpopmail[=DIR]   Include vpopmail support.])

if test "$PHP_VPOPMAIL" != "no"; then
	AC_MSG_CHECKING(for vpopmail install directory)
	for i in /usr/include/vpopmail /usr/include /usr ~vpopmail /home/vpopmail /home/popmail /var/qmail/vpopmail /var/qmail/popmail $PHP_VPOPMAIL; do
		if test -r $i/vpopmail.h; then
			VPOPMAIL_INC_DIR=$i
			VPOPMAIL_DIR=$i
		elif test -r $i/include/vpopmail.h; then
			VPOPMAIL_INC_DIR=$i/include
			VPOPMAIL_DIR=$i
		fi

		if test -r $i/libvpopmail.a; then
			VPOPMAIL_LIB_DIR=$i
		elif test -r $i/lib/libvpopmail.a; then
			VPOPMAIL_LIB_DIR=$i/lib
		fi

		if test -x $i/vadddomain; then
			VPOPMAIL_BIN_DIR=$i
		elif test -x $i/bin/vadddomain; then
			VPOPMAIL_BIN_DIR=$i/bin
		fi
	done

	for i in "$VPOPMAIL_INC_DIR/vpopmail.h" "$VPOPMAIL_INC_DIR/vpopmail_config.h" "$VPOPMAIL_LIB_DIR/libvpopmail.a"; do
		if test ! -r "$i"; then
			AC_MSG_ERROR(Could not find '$i'. Please make sure you have vpopmail installed. Use
./configure --with-vpopmail=<vpopmail-home-dir> if necessary)
		fi
	done

	for i in "$VPOPMAIL_BIN_DIR/vadddomain" "$VPOPMAIL_BIN_DIR/vaddaliasdomain" "$VPOPMAIL_BIN_DIR/vdeldomain" ; do
		if test ! -x "$i"; then
			AC_MSG_ERROR(Could not find '$i' or binary not executeable under current user.
Please make sure you have vpopmail properly installed.
Use ./configure --with-vpopmail=<vpopmail-home-dir> if necessary)
		fi
	done

	AC_MSG_RESULT($VPOPMAIL_DIR)

	dnl newer versions of vpopmail have include/vauth.h defining valias functions
	if test -r $VPOPMAIL_INC_DIR/vauth.h; then
		AC_DEFINE(HAVE_VPOPMAIL_VAUTH,1,[Whether vpopmail has vauth.h])
	fi
	dnl check for valias functions in the -lvpopmail
	PHP_CHECK_LIBRARY(vpopmail, valias_select, [
		AC_DEFINE(HAVE_VPOPMAIL_VALIAS,1,[Whether vpopmail has valias support])
	],[],[
		-L$VPOPMAIL_LIB_DIR 
	])

	PHP_ADD_INCLUDE($VPOPMAIL_INC_DIR)

	PHP_SUBST(VPOPMAIL_SHARED_LIBADD)
	PHP_ADD_LIBRARY_WITH_PATH(vpopmail, $VPOPMAIL_LIB_DIR, VPOPMAIL_SHARED_LIBADD)

	AC_DEFINE(HAVE_VPOPMAIL,1,[Whether you have vpopmail])
	AC_DEFINE_UNQUOTED(VPOPMAIL_BIN_DIR,"$VPOPMAIL_BIN_DIR",[vpopmail bin path])

	dnl Detect if we have vpopmail >= 5.2 to accomodate C-API changes
	dnl
	dnl The current table is:
	dnl
	dnl PHP API   VPOPMAIL VERSION
	dnl      1     <  5.2
	dnl      2     >= 5.2
	vpopmail_internal_api=1
	version=`grep VERSION $VPOPMAIL_INC_DIR/vpopmail_config.h`
	if test -n "$version"; then
		version_major=`echo "$version" | tr "\"#." "   " | awk '{print $3}'`
		version_minor=`echo "$version" | tr "\"#." "   " | awk '{print $4}'`
		if test $version_major -ge 5; then
			if test $version_major -eq 5; then
				if test $version_minor -ge 2; then
					vpopmail_internal_api=2
				fi
			fi
			if test $version_major -gt 5; then
				vpopmail_internal_api=2
			fi
		fi
	fi
	AC_DEFINE_UNQUOTED(HAVE_VPOPMAIL_API,$vpopmail_internal_api,[Interal definition for vpopmail API changes])

	PHP_NEW_EXTENSION(vpopmail, php_vpopmail.c, $ext_shared)
fi
