dnl ## -*- sh -*-

AC_MSG_CHECKING(for Apache 2.0 module support via DSO through APXS)
AC_ARG_WITH(apxs2,
[  --with-apxs2[=FILE]     Build shared Apache 2.0 module. FILE is the optional
                          pathname to the Apache apxs tool; defaults to "apxs".],[
	if test "$withval" = "yes"; then
		APXS=apxs
                if $APXS -q CFLAGS >/dev/null 2>&1; then
                  :
                else
                  if test -x /usr/sbin/apxs ; then #SUSE 6.x 
                    APXS=/usr/sbin/apxs
                  fi
                fi
	else
		AC_EXPAND_PATH($withval, APXS)
	fi

    if $APXS -q CFLAGS >/dev/null 2>&1; then
      :
    else
      AC_MSG_RESULT()
      $APXS
      AC_MSG_ERROR([Sorry, I cannot run apxs. Either you need to install Perl or you need to pass the absolute path of apxs by using --with-apxs2=/absolute/path/to/apxs])
    fi 

	APXS_INCLUDEDIR=`$APXS -q INCLUDEDIR`
	APXS_CFLAGS=`$APXS -q CFLAGS`
	AC_ADD_INCLUDE($APXS_INCLUDEDIR)
	AC_ADD_INCLUDE($APXS_INCLUDEDIR/apr)
	if `echo $APXS_CFLAGS|grep USE_HSREGEX>/dev/null`; then
		APACHE_HAS_REGEX=yes
	fi
	if `echo $APXS_CFLAGS|grep EAPI>/dev/null`; then
	   CPPFLAGS="$CPPFLAGS -DEAPI"
	fi
	PHP_SAPI=apache2filter
	INSTALL_IT="$APXS -i -a -n php4 $SAPI_LIBTOOL"
	PHP_BUILD_SHARED
	PHP_BUILD_THREAD_SAFE
	AC_MSG_RESULT(yes)
],[
	AC_MSG_RESULT(no)
])

PHP_SUBST(APXS)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
