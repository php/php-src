dnl
dnl $Id$
dnl

PHP_ARG_BUNDLE(bundle-expat, Whether to bundle the expat library,
[  --without-bundle-expat  Disable the bundled expat library], yes)

# BC ini option
PHP_ARG_WITH(expat-dir, external libexpat install dir,
[  --with-expat-dir=DIR    XML: external libexpat install dir], no, no)

if test "$PHP_BUNDLE_EXPAT" = "yes" && test "$PHP_EXPAT_DIR" = "no"; then
	AC_C_BIGENDIAN
	if test "$ac_cv_c_bigendian" = "yes"; then
		order=4321
	else
		order=1234
	fi

	AC_DEFINE(HAVE_LIBEXPAT_BUNDLED, 1, [ ])
	AC_DEFINE(HAVE_LIBEXPAT, 1, [ ])
	AC_DEFINE(BYTEORDER, $order, [ ])
	PHP_ADD_SOURCES(bundle/expat, xmlparse.c xmlrole.c xmltok.c)
	PHP_ADD_INCLUDE(bundle/expat)
	PHP_ADD_BUILD_DIR(bundle/expat)
elif test "$PHP_BUNDLE_EXPAT" != "no" && test "$PHP_EXPAT_DIR" != "no"; then
	# Find external expat library

	for i in $PHP_BUNDLE_EXPAT $PHP_EXPAT_DIR; do
		if test -f $i/lib/libexpat.a -o -f $i/lib/libexpat.$SHLIB_SUFFIX_NAME; then
			EXPAT_DIR=$i
		fi
	done

	if test -z "$EXPAT_DIR"; then
		AC_MSG_ERROR(not found. Please reinstall the expat distribution.)
	fi

	AC_DEFINE(HAVE_LIBEXPAT, 1, [ ])
	PHP_ADD_INCLUDE($EXPAT_DIR/include)
	PHP_ADD_LIBRARY_WITH_PATH(expat, $EXPAT_DIR/lib, EXPAT_SHARED_LIBADD)
	PHP_SUBST(EXPAT_SHARED_LIBADD)
fi
