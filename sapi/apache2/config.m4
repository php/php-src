dnldnl ## -*- sh -*-
dnl
dnlAC_DEFUN(PHP_APACHE_CHECK_RDYNAMIC,[
dnl  if test -n "$GCC"; then
dnl    dnl we should use a PHP-specific macro here
dnl    TSRM_CHECK_GCC_ARG(-rdynamic, gcc_rdynamic=yes)
dnl    if test "$gcc_rdynamic" = "yes"; then
dnl      PHP_LDFLAGS="$PHP_LDFLAGS -rdynamic"
dnl    fi
dnl  fi
dnl])
dnl
dnlAC_MSG_CHECKING(for Apache module support via DSO through APXS)
dnlAC_ARG_WITH(apxs,
dnl[  --with-apxs[=FILE]      Build shared Apache module. FILE is the optional
dnl                          pathname to the Apache apxs tool; defaults to "apxs".],[
dnl	if test "$withval" = "yes"; then
dnl		APXS=apxs
dnl                if $APXS -q CFLAGS >/dev/null 2>&1; then
dnl                  :
dnl                else
dnl                  if test -x /usr/sbin/apxs ; then #SUSE 6.x 
dnl                    APXS=/usr/sbin/apxs
dnl                  fi
dnl                fi
dnl	else
dnl		AC_EXPAND_PATH($withval, APXS)
dnl	fi
dnl
dnl    if $APXS -q CFLAGS >/dev/null 2>&1; then
dnl      :
dnl    else
dnl      AC_MSG_RESULT()
dnl      $APXS
dnl      AC_MSG_ERROR([Sorry, I cannot run apxs. Either you need to install Perl or you need to pass the absolute path of apxs by using --with-apxs=/absolute/path/to/apxs])
dnl    fi 
dnl
dnl	APXS_LDFLAGS="@SYBASE_LFLAGS@ @SYBASE_LIBS@ @SYBASE_CT_LFLAGS@ @SYBASE_CT_LIBS@"
dnl	APXS_INCLUDEDIR=`$APXS -q INCLUDEDIR`
dnl	APXS_CFLAGS=`$APXS -q CFLAGS`
dnl	AC_ADD_INCLUDE($APXS_INCLUDEDIR)
dnl	if `echo $APXS_CFLAGS|grep USE_HSREGEX>/dev/null`; then
dnl		APACHE_HAS_REGEX=yes
dnl	fi
dnl	if `echo $APXS_CFLAGS|grep EAPI>/dev/null`; then
dnl	   CPPFLAGS="$CPPFLAGS -DEAPI"
dnl	fi
dnl	PHP_SAPI=apache
dnl	APACHE_INSTALL="$APXS -i -a -n php4 $SAPI_SHARED"
dnl	PHP_BUILD_SHARED
dnl	if test -z "`$APXS -q LD_SHLIB`" || test "`$APXS -q LIBEXECDIR`" = "modules"; then
dnl		PHP_APXS_BROKEN=yes
dnl	fi
dnl	STRONGHOLD=
dnl	AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
dnl	AC_DEFINE(HAVE_AP_COMPAT_H,1,[ ])
dnl	AC_MSG_RESULT(yes)
dnl],[
dnl	AC_MSG_RESULT(no)
dnl])
dnl
dnlAPACHE_INSTALL_FILES="\$(srcdir)/sapi/apache/mod_php4.* sapi/apache/libphp4.module"
dnl
dnlif test "$PHP_SAPI" != "apache"; then
dnlAC_MSG_CHECKING(for Apache module support)
dnlAC_ARG_WITH(apache,
dnl[  --with-apache[=DIR]     Build Apache module. DIR is the top-level Apache
dnl                          build directory, defaults to /usr/local/etc/httpd.],[
dnl	if test "$withval" = "yes"; then
dnl	  # Apache's default directory
dnl	  withval=/usr/local/apache
dnl	fi
dnl	if test "$withval" != "no"; then
dnl		APACHE_MODULE=yes
dnl		AC_EXPAND_PATH($withval, withval)
dnl		# For Apache 1.2.x
dnl		if test -f $withval/src/httpd.h; then 
dnl			APACHE_INCLUDE=-I$withval/src
dnl			APACHE_TARGET=$withval/src
dnl			PHP_SAPI=apache
dnl			APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_INSTALL_FILES $APACHE_TARGET"
dnl			PHP_LIBS="-L. -lphp3"
dnl			AC_MSG_RESULT(yes - Apache 1.2.x)
dnl			STRONGHOLD=
dnl			if test -f $withval/src/ap_config.h; then
dnl				AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
dnl			fi
dnl		# For Apache 2.0.x
dnl		elif test -f $withval/src/include/httpd.h &&
dnl		     test -f $withval/src/lib/apr/include/apr_general.h ; then
dnl			APACHE_HAS_REGEX=1
dnl			APACHE_INCLUDE="-I$withval/src/include -I$withval/src/os/unix -I$withval/src/lib/apr/include"
dnl			APACHE_TARGET=$withval/src/modules/php4
dnl			if test ! -d $APACHE_TARGET; then
dnl				mkdir $APACHE_TARGET
dnl			fi
dnl			PHP_SAPI=apache
dnl			APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp4.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET; cp $srcdir/sapi/apache/apMakefile.tmpl $APACHE_TARGET/Makefile.tmpl; cp $srcdir/sapi/apache/apMakefile.libdir $APACHE_TARGET/Makefile.libdir"
dnl			PHP_LIBS="-Lmodules/php4 -L../modules/php4 -L../../modules/php4 -lmodphp4"
dnl			AC_MSG_RESULT(yes - Apache 2.0.X)
dnl			STRONGHOLD=
dnl			if test -f $withval/src/include/ap_config.h; then
dnl				AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
dnl			fi
dnl			if test -f $withval/src/include/ap_compat.h; then
dnl				AC_DEFINE(HAVE_AP_COMPAT_H,1,[ ])
dnl				if test ! -f $withval/src/include/ap_config_auto.h; then
dnl					AC_MSG_ERROR(Please run Apache\'s configure or src/Configure program once and try again)
dnl				fi
dnl			else
dnl				if test -f $withval/src/include/compat.h; then
dnl					AC_DEFINE(HAVE_OLD_COMPAT_H,1,[ ])
dnl				fi
dnl			fi
dnl		# For Apache 1.3.x
dnl		elif test -f $withval/src/main/httpd.h; then
dnl			APACHE_HAS_REGEX=1
dnl			APACHE_INCLUDE="-I$withval/src/main -I$withval/src/os/unix -I$withval/src/ap"
dnl			APACHE_TARGET=$withval/src/modules/php4
dnl			if test ! -d $APACHE_TARGET; then
dnl				mkdir $APACHE_TARGET
dnl			fi
dnl			PHP_SAPI=apache
dnl			APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp4.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET; cp $srcdir/sapi/apache/apMakefile.tmpl $APACHE_TARGET/Makefile.tmpl; cp $srcdir/sapi/apache/apMakefile.libdir $APACHE_TARGET/Makefile.libdir"
dnl			PHP_LIBS="-Lmodules/php4 -L../modules/php4 -L../../modules/php4 -lmodphp4"
dnl			AC_MSG_RESULT(yes - Apache 1.3.x)
dnl			STRONGHOLD=
dnl			if test -f $withval/src/include/ap_config.h; then
dnl				AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
dnl			fi
dnl			if test -f $withval/src/include/ap_compat.h; then
dnl				AC_DEFINE(HAVE_AP_COMPAT_H,1,[ ])
dnl				if test ! -f $withval/src/include/ap_config_auto.h; then
dnl					AC_MSG_ERROR(Please run Apache\'s configure or src/Configure program once and try again)
dnl				fi
dnl			else
dnl				if test -f $withval/src/include/compat.h; then
dnl					AC_DEFINE(HAVE_OLD_COMPAT_H,1,[ ])
dnl				fi
dnl			fi
dnl		# Also for Apache 1.3.x
dnl		elif test -f $withval/src/include/httpd.h; then
dnl			APACHE_HAS_REGEX=1
dnl			APACHE_INCLUDE="-I$withval/src/include -I$withval/src/os/unix"
dnl			APACHE_TARGET=$withval/src/modules/php4
dnl			if test ! -d $APACHE_TARGET; then
dnl				mkdir $APACHE_TARGET
dnl			fi
dnl			PHP_SAPI=apache
dnl			PHP_LIBS="-Lmodules/php4 -L../modules/php4 -L../../modules/php4 -lmodphp4"
dnl			APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp4.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET; cp $srcdir/sapi/apache/apMakefile.tmpl $APACHE_TARGET/Makefile.tmpl; cp $srcdir/sapi/apache/apMakefile.libdir $APACHE_TARGET/Makefile.libdir"
dnl			AC_MSG_RESULT(yes - Apache 1.3.x)
dnl			STRONGHOLD=
dnl			if test -f $withval/src/include/ap_config.h; then
dnl				AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
dnl			fi
dnl			if test -f $withval/src/include/ap_compat.h; then
dnl				AC_DEFINE(HAVE_AP_COMPAT_H,1,[ ])
dnl				if test ! -f $withval/src/include/ap_config_auto.h; then
dnl					AC_MSG_ERROR(Please run Apache\'s configure or src/Configure program once and try again)
dnl				fi
dnl			else
dnl				if test -f $withval/src/include/compat.h; then
dnl					AC_DEFINE(HAVE_OLD_COMPAT_H,1,[ ])
dnl				fi
dnl			fi
dnl		# For StrongHold 2.2
dnl		elif test -f $withval/apache/httpd.h; then
dnl			APACHE_INCLUDE=-"I$withval/apache -I$withval/ssl/include"
dnl			APACHE_TARGET=$withval/apache
dnl			PHP_SAPI=apache
dnl			PHP_LIBS="-Lmodules/php4 -L../modules/php4 -L../../modules/php4 -lmodphp4"
dnl			APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp4.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET"
dnl			STRONGHOLD=-DSTRONGHOLD=1
dnl			AC_MSG_RESULT(yes - StrongHold)
dnl			if test -f $withval/apache/ap_config.h; then
dnl				AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
dnl			fi
dnl			if test -f $withval/src/ap_compat.h; then
dnl				AC_DEFINE(HAVE_AP_COMPAT_H,1,[ ])
dnl				if test ! -f $withval/src/include/ap_config_auto.h; then
dnl					AC_MSG_ERROR(Please run Apache\'s configure or src/Configure program once and try again)
dnl				fi
dnl			else
dnl				if test -f $withval/src/compat.h; then
dnl					AC_DEFINE(HAVE_OLD_COMPAT_H,1,[ ])
dnl				fi
dnl			fi
dnl		else
dnl			AC_MSG_RESULT(no)
dnl			AC_MSG_ERROR(Invalid Apache directory - unable to find httpd.h under $withval)
dnl		fi
dnl	else
dnl		AC_MSG_RESULT(no)
dnl	fi
dnl],[
dnl	AC_MSG_RESULT(no)
dnl])
dnl
dnlINCLUDES="$INCLUDES $APACHE_INCLUDE"
dnlfi
dnl
dnlif test "x$APXS" != "x" -a "`uname -sv`" = "AIX 4" -a "$GCC" != "yes"; then
dnl	APXS_EXP="-bE:sapi/apache/mod_php4.exp"
dnlfi
dnl
dnlPHP_SUBST(APXS_EXP)
dnlPHP_SUBST(APACHE_INCLUDE)
dnlPHP_SUBST(APACHE_TARGET)
dnlPHP_SUBST(APXS)
dnlPHP_SUBST(APXS_LDFLAGS)
dnlPHP_SUBST(APACHE_INSTALL)
dnlPHP_SUBST(STRONGHOLD)
dnl
dnlAC_MSG_CHECKING(for mod_charset compatibility option)
dnlAC_ARG_WITH(mod_charset,
dnl[  --with-mod_charset      Enable transfer tables for mod_charset (Rus Apache).],
dnl[
dnl	AC_MSG_RESULT(yes)
dnl    AC_DEFINE(USE_TRANSFER_TABLES,1,[ ])
dnl],[
dnl	AC_MSG_RESULT(no)
dnl])
dnl
dnlif test -n "$APACHE_MODULE"; then
dnl  PHP_APACHE_CHECK_RDYNAMIC
dnl  $php_shtool mkdir -p sapi/apache
dnl  PHP_OUTPUT(sapi/apache/libphp4.module)
dnl  PHP_BUILD_STATIC
dnlfi
dnl
dnlif test -n "$APACHE_INSTALL"; then
dnl  INSTALL_IT=$APACHE_INSTALL
dnlfi
dnl
dnldnl ## Local Variables:
dnldnl ## tab-width: 4
dnldnl ## End:
