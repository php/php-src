dnl $Id$

dnl My first config.m4 - be nice to me... :-)

PHP_ARG_ENABLE(satellite, whether to enable CORBA support via Satellite,
dnl Make sure that the comment is aligned:
[  --enable-satellite      Enable CORBA support via Satellite (Requires ORBit)])

if test "$PHP_SATELLITE" != "no"; then

	dnl check for orbit-config
	AC_PATH_PROG(orbit_config, "orbit-config")
	
	if test -z "$orbit_config"; then
		AC_MSG_ERROR(Cannot find orbit-config, install ORBit!)
	fi

	dnl check for symbol giop_skip_atexit in libIIOP
	AC_CHECK_LIB(IIOP, giop_skip_atexit, [true], [
		AC_MSG_ERROR([You need a CVS version of ORBit, please read ext/satallite/README])
		], `$orbit_config --libs client`)

	dnl ORBIT_CFLAGS=`$orbit_config --cflags client`
	dnl ORBIT_LIBS=`$orbit_config --libs client`

	dnl check for libIDL-config
	AC_PATH_PROG(libidl_config, "libIDL-config")

	if test -z "libidl_config"; then
		AC_MSG_ERROR(Cannot find libIDL-config, install libIDL!)
	fi

	dnl LIBIDL_CFLAGS=`$libidl_config --cflags`
	dnl LIBIDL_LIBS=`$libidl_config --libs`

	dnl CFLAGS="$CFLAGS $ORBIT_CFLAGS $LIBIDL_CFLAGS"

  AC_DEFINE(HAVE_SATELLITE, 1, [CORBA support enabled via Satellite])
  PHP_EXTENSION(satellite, $ext_shared)
fi
