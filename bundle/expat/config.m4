PHP_ARG_BUNDLE(bundle-expat, Whether to enable XML support,
[  --disable-bundle-expat  Use the bundled expat library], yes)

if test "$PHP_BUNDLE_EXPAT" = "yes"; then
	AC_C_BIGENDIAN
	if test "$ac_cv_c_bigendian" = "yes"; then
		order=4321
	else
		order=1234
	fi

	AC_DEFINE(HAVE_LIBEXPAT_BUNDLED, 1, [ ])
	AC_DEFINE(BYTEORDER, $order, [ ])
	PHP_ADD_SOURCES(bundle/expat, xmlparse.c xmlrole.c xmltok.c)
	PHP_ADD_INCLUDE(bundle/expat)
	PHP_ADD_BUILD_DIR(bundle/expat)
fi
