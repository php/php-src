dnl ## $Id$ -*- sh -*-

RESULT=no
AC_MSG_CHECKING(for Caudium support)
AC_ARG_WITH(caudium, 
[  --with-caudium=DIR      Build PHP as a Pike module for use with Caudium
                          DIR is the Caudium server dir, with the default value
						  /usr/local/caudium/server.],
[
	if test ! -d $withval ; then
		withval=/usr/local/caudium/server
	fi
	if test -f $withval/bin/caudium; then
		PIKE="$withval/bin/caudium"
	elif test -f $withval/bin/pike; then
		PIKE="$withval/bin/pike"
	else
		AC_MSG_ERROR(Couldn't find a pike in $withval/bin/)
	fi
    if $PIKE -e 'float v; int rel;sscanf(version(), "Pike v%f release %d", v, rel);v += rel/10000.0; if(v < 7.0) exit(1); exit(0);'; then
		PIKE_MODULE_DIR="`$PIKE --show-paths 2>&1| grep '^Module' | sed -e 's/.*: //'`"
	    PIKE_INCLUDE_DIR="`echo $PIKE_MODULE_DIR | sed -e 's,lib/pike/modules,include/pike,' -e 's,lib/modules,include/pike,'`"
		if test -z "$PIKE_INCLUDE_DIR" -o -z "$PIKE_MODULE_DIR"; then
			AC_MSG_ERROR(Failed to figure out Pike module and include directories)
		fi
	else
		AC_MSG_ERROR(Caudium PHP4 requires Pike 7.0 or newer)
	fi
    PIKE_VERSION=`$PIKE -e 'string v; int rel;sscanf(version(), "Pike v%s release %d", v, rel); write(v+"."+rel);'`   
	AC_ADD_INCLUDE($PIKE_INCLUDE_DIR)
	AC_DEFINE(HAVE_CAUDIUM,1,[Whether to compile with Caudium support])
	PHP_SAPI=caudium
	PHP_BUILD_SHARED
	INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED $withval/lib/$PIKE_VERSION/PHP4.so"
	RESULT="yes
	*** Pike binary used:      $PIKE
	*** Pike include dir:      $PIKE_INCLUDE_DIR
	*** Pike version:          $PIKE_VERSION"
    PIKE_INCLUDE_DIR=" -I$PIKE_INCLUDE_DIR "
	
    dnl Always use threads since thread-free support really blows.
    PHP_BUILD_THREAD_SAFE

])
AC_MSG_RESULT($RESULT)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
	
