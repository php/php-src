dnl $Id$
dnl vim: syntax=config

dnl My first config.m4 - now much improved... :-)

PHP_ARG_WITH(satellite, whether to enable CORBA support via Satellite,
dnl Make sure that the comment is aligned:
[  --with-satellite[=DIR]  Enable CORBA support via Satellite (EXPERIMENTIAL)
                          DIR is the base directory for ORBit])

if test "$PHP_SATELLITE" != "no"; then
  
  dnl So, we have an extension called satellite
	PHP_EXTENSION(satellite, $ext_shared)

  dnl We want to use these variables in Makefile.in
  PHP_SUBST(SATELLITE_SHARED_LIBADD)

  dnl Now, where is ORBit?
  AC_MSG_CHECKING(for ORBit)

  dnl These directories look reasonable
  for i in $PHP_SATELLITE /usr/local/orbit /usr/local /usr; do

    dnl Does this look like an ORBit installation?
    if test -e $i/include/orb/orbit.h; then

      dnl Save directory and report result
      ORBIT_PATH=$i
      AC_MSG_RESULT(found in $i)

    fi
  done

  dnl Abort if the loop above failed
  if test -z "$ORBIT_PATH"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please (re)install ORBit)
  fi

	dnl
	dnl SATELLITE_CONFIG(config-name, path, [extra], message)
	dnl
	AC_DEFUN(SATELLITE_CONFIG,[

		dnl look for config program
		AC_PATH_PROG(satellite_$1, $1-config, , $2)
		if test -z "$satellite_$1"; then 
			AC_MSG_ERROR($4)
		fi

		dnl get cflags
		satellite_cflags=`$satellite_$1 --cflags $3`
		if test -z "$satellite_cflags"; then
			AC_MSG_ERROR($4)
		fi
		PHP_EVAL_INCLINE($satellite_cflags)
	
		dnl get libs
		satellite_libs=`$satellite_$1 --libs $3`
		if test -z "$satellite_libs"; then
			AC_MSG_ERROR($4)
		fi
		PHP_EVAL_LIBLINE($satellite_libs)
	])
	
	SATELLITE_CONFIG(orbit, $ORBIT_PATH/bin, [client], [Please (re)install ORBit])
	SATELLITE_CONFIG(libIDL, $ORBIT_PATH/bin, , [Please (re)install libIDL])
	
	dnl check for symbol giop_skip_atexit in libIIOP
	AC_CHECK_LIB(IIOP, giop_skip_atexit, [true], [
		AC_MSG_ERROR([You need a CVS version of ORBit, please read ext/satellite/README])
		], "$ORBIT_LIBS" )

  AC_DEFINE(HAVE_SATELLITE,1,[ ])
fi
