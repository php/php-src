dnl ## -*- sh -*-

AC_MSG_CHECKING(for Apache module support via DSO through APXS)
AC_ARG_WITH(apxs,
[  --with-apxs[=FILE]	   Build shared Apache module.	FILE is the optional
						  pathname to the Apache apxs tool; defaults to "apxs".],
[
	if test "$withval" = "yes"; then
		APXS=apxs
	else
		AC_EXPAND_PATH($withval, APXS)
	fi
	APXS_LDFLAGS="@SYBASE_LFLAGS@ @SYBASE_LIBS@ @SYBASE_CT_LFLAGS@ @SYBASE_CT_LIBS@"
	APXS_INCLUDEDIR=`$APXS -q INCLUDEDIR`
	if test -d "$APXS_INCLUDEDIR/xml" ; then
		XML_INCLUDE="$APXS_INCLUDEDIR/xml"
	fi
	AC_ADD_INCLUDE($APXS_INCLUDEDIR)
    PHP_EXTENSION(apache)
	PHP_SAPI=apache
    APACHE_INSTALL="$APXS -i -a -n php4 $SAPI_SHARED"
	PHP_BUILD_SHARED
	STRONGHOLD=
	AC_DEFINE(APACHE)
	AC_DEFINE(HAVE_AP_CONFIG_H)
	AC_DEFINE(HAVE_AP_COMPAT_H)
	AC_MSG_RESULT(yes)
],[
	AC_MSG_RESULT(no)
])

APACHE_INSTALL_FILES="\$(srcdir)/sapi/apache/mod_php4.* sapi/apache/libphp4.module"

if test "$PHP_SAPI" != "apache"; then
AC_MSG_CHECKING(for Apache module support)
AC_ARG_WITH(apache,
[  --with-apache[=DIR]	   Build Apache module.	 DIR is the top-level Apache
						  build directory, defaults to /usr/local/etc/httpd.],
[
	if test "$withval" = "yes"; then
	  # Apache's default directory
	  withval=/usr/local/apache
	fi
	if test "$withval" != "no"; then
		APACHE_MODULE=yes
		AC_EXPAND_PATH($withval, withval)
		# For Apache 1.2.x
		if test -f $withval/src/httpd.h; then 
			APACHE_INCLUDE=-I$withval/src
			APACHE_TARGET=$withval/src
			PHP_SAPI=apache
			APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_INSTALL_FILES $APACHE_TARGET"
			PHP_LIBS="-L. -lphp3"
			AC_DEFINE(APACHE)
			AC_MSG_RESULT(yes - Apache 1.2.x)
			STRONGHOLD=
			if test -f $withval/src/ap_config.h; then
				AC_DEFINE(HAVE_AP_CONFIG_H)
			fi
		# For Apache 1.3.x
		elif test -f $withval/src/main/httpd.h; then
			APACHE_HAS_REGEX=1
			APACHE_INCLUDE="-I$withval/src/main -I$withval/src/os/unix -I$withval/src/ap"
			APACHE_TARGET=$withval/src/modules/php4
			if test ! -d $APACHE_TARGET; then
				mkdir $APACHE_TARGET
			fi
			PHP_SAPI=apache
			APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp4.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET; cp $srcdir/apMakefile.tmpl $APACHE_TARGET/Makefile.tmpl; cp $srcdir/apMakefile.libdir $APACHE_TARGET/Makefile.libdir"
			PHP_LIBS="-Lmodules/php4 -L../modules/php4 -L../../modules/php4 -lmodphp4"
			AC_DEFINE(APACHE)
			AC_MSG_RESULT(yes - Apache 1.3.x)
			STRONGHOLD=
			if test -f $withval/src/include/ap_config.h; then
				AC_DEFINE(HAVE_AP_CONFIG_H)
			fi
			if test -f $withval/src/include/ap_compat.h; then
				AC_DEFINE(HAVE_AP_COMPAT_H)
				if test ! -f $withval/src/include/ap_config_auto.h; then
					AC_MSG_ERROR(Please run Apache\'s configure or src/Configure program once and try again)
				fi
			else
				if test -f $withval/src/include/compat.h; then
					AC_DEFINE(HAVE_OLD_COMPAT_H)
				fi
			fi
		# Also for Apache 1.3.x
		elif test -f $withval/src/include/httpd.h; then
			APACHE_HAS_REGEX=1
			APACHE_INCLUDE="-I$withval/src/include -I$withval/src/os/unix"
			APACHE_TARGET=$withval/src/modules/php4
			if test -d $withval/src/lib/expat-lite ; then
				  XML_INCLUDE=$withval/src/lib/expat-lite
			fi
			if test ! -d $APACHE_TARGET; then
				mkdir $APACHE_TARGET
			fi
			PHP_SAPI=apache
			PHP_LIBS="-Lmodules/php4 -L../modules/php4 -L../../modules/php4 -lmodphp4"
			APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp4.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET; cp $srcdir/apMakefile.tmpl $APACHE_TARGET/Makefile.tmpl; cp $srcdir/apMakefile.libdir $APACHE_TARGET/Makefile.libdir"
			AC_DEFINE(APACHE)
			AC_MSG_RESULT(yes - Apache 1.3.x)
			STRONGHOLD=
			if test -f $withval/src/include/ap_config.h; then
				AC_DEFINE(HAVE_AP_CONFIG_H)
			fi
			if test -f $withval/src/include/ap_compat.h; then
				AC_DEFINE(HAVE_AP_COMPAT_H)
				if test ! -f $withval/src/include/ap_config_auto.h; then
					AC_MSG_ERROR(Please run Apache\'s configure or src/Configure program once and try again)
				fi
			else
				if test -f $withval/src/include/compat.h; then
					AC_DEFINE(HAVE_OLD_COMPAT_H)
				fi
			fi
		# For StrongHold 2.2
		elif test -f $withval/apache/httpd.h; then
			APACHE_INCLUDE=-"I$withval/apache -I$withval/ssl/include"
			APACHE_TARGET=$withval/apache
			PHP_SAPI=apache
			PHP_LIBS="-Lmodules/php4 -L../modules/php4 -L../../modules/php4 -lmodphp4"
			APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp4.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET"
			STRONGHOLD=-DSTRONGHOLD=1
			AC_DEFINE(APACHE)
			AC_MSG_RESULT(yes - StrongHold)
			if test -f $withval/apache/ap_config.h; then
				AC_DEFINE(HAVE_AP_CONFIG_H)
			fi
			if test -f $withval/src/ap_compat.h; then
				AC_DEFINE(HAVE_AP_COMPAT_H)
				if test ! -f $withval/src/include/ap_config_auto.h; then
					AC_MSG_ERROR(Please run Apache\'s configure or src/Configure program once and try again)
				fi
			else
				if test -f $withval/src/compat.h; then
					AC_DEFINE(HAVE_OLD_COMPAT_H)
				fi
			fi
		else
			AC_MSG_RESULT(no)
			AC_MSG_ERROR(Invalid Apache directory - unable to find httpd.h under $withval)
		fi
	else
		AC_MSG_RESULT(no)
	fi
],[
	AC_MSG_RESULT(no)
])

INCLUDES="$INCLUDES $APACHE_INCLUDE"
dnl## AC_SUBST(APACHE_INCLUDE)
fi

if test "x$APXS" != "x" -a "`uname -sv`" = "AIX 4" -a "$GCC" != "yes"; then
	APXS_EXP="-bE:sapi/apache/mod_php4.exp"
fi

AC_SUBST(APXS_EXP)
AC_SUBST(APACHE_INCLUDE)
AC_SUBST(APACHE_TARGET)
AC_SUBST(APXS)
AC_SUBST(APXS_LDFLAGS)
AC_SUBST(APACHE_INSTALL)
AC_SUBST(STRONGHOLD)

AC_MSG_CHECKING(for mod_charset compatibility option)
AC_ARG_WITH(mod_charset,
[  --with-mod_charset      Enable transfer tables for mod_charset (Rus Apache).],
[
	AC_MSG_RESULT(yes)
    AC_DEFINE(USE_TRANSFER_TABLES)
],[
	AC_MSG_RESULT(no)
])

if test "$with_regex" = "apache" && test -z "$APACHE_HAS_REGEX"; then
  with_regex=php
fi

if test -z "$with_regex" && test -n "$APACHE_HAS_REGEX"; then
  with_regex=apache
fi

if test -n "$APACHE_MODULE"; then
  if test "$with_regex" = "apache"; then
    APACHE_WANT_HSREGEX=yes
  else
    APACHE_WANT_HSREGEX=no
  fi
  AC_SUBST(APACHE_WANT_HSREGEX)
  PHP_EXTENSION(apache)
  PHP_OUTPUT(sapi/apache/libphp4.module)
  PHP_BUILD_STATIC
fi

if test -n "$APACHE_INSTALL"; then
  INSTALL_IT=$APACHE_INSTALL
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
