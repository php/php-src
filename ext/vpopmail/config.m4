dnl $Id$
dnl config.m4 for extension vpopmail

PHP_ARG_WITH(vpopmail, for vpopmail support,
[  --with-vpopmail[=DIR]   Include vpopmail support.])

if test "$PHP_VPOPMAIL" != "no"; then
	AC_MSG_CHECKING(for vpopmail install directory)
	for i in ~vpopmail /home/vpopmail /home/popmail /var/qmail/vpopmail /var/qmail/popmail $PHP_VPOPMAIL; do
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

		if test -r $i/vadddomain; then
			VPOPMAIL_BIN_DIR=$i
		elif test -r $i/bin/vadddomain; then
			VPOPMAIL_BIN_DIR=$i/bin
		fi
	done

	for i in "$VPOPMAIL_INC_DIR/vpopmail.h" "$VPOPMAIL_INC_DIR/vpopmail_config.h" "$VPOPMAIL_LIB_DIR/libvpopmail.a" "$VPOPMAIL_BIN_DIR/vadddomain" "$VPOPMAIL_BIN_DIR/vaddaliasdomain" "$VPOPMAIL_BIN_DIR/vdeldomain" ; do
		if test ! -r "$i"; then
			AC_MSG_ERROR(Could not find '$i'. Please make sure you have
				vpopmail installed. Use
				./configure --with-vpopmail=<vpopmail-home-dir> if necessary)
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
	],[],
		-L$VPOPMAIL_LIB_DIR 
	])

	PHP_ADD_INCLUDE($VPOPMAIL_INC_DIR)

	PHP_SUBST(VPOPMAIL_SHARED_LIBADD)
	PHP_ADD_LIBRARY_WITH_PATH(vpopmail, $VPOPMAIL_LIB_DIR, VPOPMAIL_SHARED_LIBADD)

	AC_DEFINE(HAVE_VPOPMAIL,1,[Whether you have vpopmail])
	AC_DEFINE_UNQUOTED(VPOPMAIL_BIN_DIR,"$VPOPMAIL_BIN_DIR",[vpopmail bin path])

	PHP_EXTENSION(vpopmail, $ext_shared)
fi
