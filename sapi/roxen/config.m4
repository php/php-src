dnl
dnl $Id$
dnl

RESULT=no
AC_MSG_CHECKING(for Roxen/Pike support)
AC_ARG_WITH(roxen, 
[  --with-roxen=DIR        Build PHP as a Pike module. DIR is the base Roxen
                          directory, normally /usr/local/roxen/server.],
[
	if test ! -d $withval ; then
		AC_MSG_ERROR(You did not specify a directory)
	fi
	if test -f $withval/bin/roxen; then
		PIKE=$withval/bin/roxen
	elif test -f $withval/bin/pike; then
		PIKE=$withval/bin/pike
	else
		AC_MSG_ERROR(Couldn't find a pike in $withval/bin/)
	fi
    if $PIKE -e 'float v; catch(v = __VERSION__ + (__BUILD__/10000.0)); if(v < 0.7079) exit(1); exit(0);'; then
		PIKE_MODULE_DIR=`$PIKE --show-paths 2>&1| grep '^Module' | sed -e 's/.*: //'`
	    PIKE_INCLUDE_DIR=`echo $PIKE_MODULE_DIR | sed -e 's,lib/pike/modules,include/pike,' -e 's,lib/modules,include/pike,'`
		if test -z "$PIKE_INCLUDE_DIR" -o -z "$PIKE_MODULE_DIR"; then
			AC_MSG_ERROR(Failed to figure out Pike module and include directories)
		fi
	else
		AC_MSG_ERROR(Roxen/PHP requires Pike 0.7.79 or newer)
	fi
   
	PHP_ADD_INCLUDE($PIKE_INCLUDE_DIR)
	AC_DEFINE(HAVE_ROXEN,1,[Whether you use Roxen])
	PHP_SAPI=roxen
	PHP_BUILD_SHARED
	INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED $PIKE_MODULE_DIR/PHP4.so"
	RESULT="yes
	Pike binary used:      $PIKE
	Pike include dir:      $PIKE_INCLUDE_DIR
	Pike module directory: $PIKE_MODULE_DIR"
	PIKE_INCLUDE_DIR=" -I$PIKE_INCLUDE_DIR "
])
AC_MSG_RESULT($RESULT)

if test "$RESULT" != "no" ; then 
 RESULT=no
 AC_MSG_CHECKING(if Roxen should use ZTS)
 AC_ARG_ENABLE(roxen-zts, 
 [  --enable-roxen-zts      Build the Roxen module using Zend Thread Safety.],
 [
 	PHP_BUILD_THREAD_SAFE
	AC_DEFINE(ROXEN_USE_ZTS,1,[Whether to use Roxen in ZTS mode])
	RESULT=yes

 ])
 AC_MSG_RESULT($RESULT)
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
	
