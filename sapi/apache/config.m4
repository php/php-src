dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for Apache 1.x module support via DSO through APXS)
AC_ARG_WITH(apxs,
[  --with-apxs[=FILE]      Build shared Apache 1.x module. FILE is the optional
                          pathname to the Apache apxs tool; defaults to "apxs".],[
  if test "$withval" = "yes"; then
    APXS=apxs
    $APXS -q CFLAGS >/dev/null 2>&1
    if test "$?" != "0" && test -x /usr/sbin/apxs; then #SUSE 6.x 
      APXS=/usr/sbin/apxs
    fi
  else
    PHP_EXPAND_PATH($withval, APXS)
  fi

  $APXS -q CFLAGS >/dev/null 2>&1
  if test "$?" != "0"; then
    AC_MSG_RESULT()
    AC_MSG_RESULT()
    AC_MSG_RESULT([Sorry, I was not able to successfully run APXS.  Possible reasons:])
    AC_MSG_RESULT()
    AC_MSG_RESULT([1.  Perl is not installed;])
    AC_MSG_RESULT([2.  Apache was not compiled with DSO support (--enable-module=so);])
    AC_MSG_RESULT([3.  'apxs' is not in your path.  Try to use --with-apxs=/path/to/apxs])
    AC_MSG_RESULT([The output of $APXS follows])
    $APXS
    AC_MSG_ERROR([Aborting]) 
  fi 

  APXS_LDFLAGS="@SYBASE_LFLAGS@ @SYBASE_LIBS@ @SYBASE_CT_LFLAGS@ @SYBASE_CT_LIBS@"
  APXS_INCLUDEDIR=`$APXS -q INCLUDEDIR`
  APXS_CFLAGS=`$APXS -q CFLAGS`
  APXS_HTTPD=`$APXS -q SBINDIR`/`$APXS -q TARGET`

  # Test that we're trying to configure with apache 1.x
  PHP_AP_EXTRACT_VERSION($APXS_HTTPD)
  if test "$APACHE_VERSION" -ge 2000000; then
    AC_MSG_ERROR([You have enabled Apache 1.3 support while your server is Apache 2.  Please use the appropiate switch --with-apxs2]) 
  fi

  for flag in $APXS_CFLAGS; do
    case $flag in
    -D*) CPPFLAGS="$CPPFLAGS $flag";;
    esac
  done
  case $host_alias in
  *aix*)
    APXS_LIBEXECDIR=`$APXS -q LIBEXECDIR`
    EXTRA_LDFLAGS="$EXTRA_LDFLAGS -Wl,-bI:$APXS_LIBEXECDIR/httpd.exp"
    PHP_SELECT_SAPI(apache, shared, sapi_apache.c mod_php4.c php_apache.c, -I$APXS_INCLUDEDIR)
    ;;
  *darwin*)
    MH_BUNDLE_FLAGS="-dynamic -twolevel_namespace -bundle -bundle_loader $APXS_HTTPD"
    PHP_SUBST(MH_BUNDLE_FLAGS)
    SAPI_SHARED=libs/libphp4.so
    PHP_SELECT_SAPI(apache, bundle, sapi_apache.c mod_php4.c php_apache.c, -I$APXS_INCLUDEDIR)
    ;;
  *)
    PHP_SELECT_SAPI(apache, shared, sapi_apache.c mod_php4.c php_apache.c, -I$APXS_INCLUDEDIR)
    ;;
  esac

  # Test whether apxs support -S option
  $APXS -q -S CFLAGS="$APXS_CFLAGS" CFLAGS >/dev/null 2>&1

  if test "$?" != "0"; then
    APACHE_INSTALL="$APXS -i -a -n php4 $SAPI_SHARED" # Old apxs does not have -S option
  else 
    APACHE_INSTALL="\$(mkinstalldirs) \"\$(INSTALL_ROOT)`$APXS -q LIBEXECDIR`\" && $APXS -S LIBEXECDIR=\"\$(INSTALL_ROOT)`$APXS -q LIBEXECDIR`\" -i -a -n php4 $SAPI_SHARED"
  fi

  if test -z "`$APXS -q LD_SHLIB`" || test "`$APXS -q LIBEXECDIR`" = "modules"; then
    PHP_APXS_BROKEN=yes
  fi
  STRONGHOLD=
  AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
  AC_DEFINE(HAVE_AP_COMPAT_H,1,[ ])
  AC_MSG_RESULT(yes)
],[
  AC_MSG_RESULT(no)
])

APACHE_INSTALL_FILES="\$(srcdir)/sapi/apache/mod_php4.* sapi/apache/libphp4.module"

if test "$PHP_SAPI" != "apache"; then
AC_MSG_CHECKING(for Apache 1.x module support)
AC_ARG_WITH(apache,
[  --with-apache[=DIR]     Build Apache 1.x module. DIR is the top-level Apache
                          build directory, defaults to /usr/local/apache.],[
  if test "$withval" = "yes"; then
    # Apache's default directory
    withval=/usr/local/apache
  fi
  if test "$withval" != "no"; then
    APACHE_MODULE=yes
    PHP_EXPAND_PATH($withval, withval)
    # For Apache 1.2.x
    if test -f $withval/src/httpd.h; then 
      APACHE_INCLUDE=-I$withval/src
      APACHE_TARGET=$withval/src
      PHP_SELECT_SAPI(apache, static, sapi_apache.c mod_php4.c php_apache.c, $APACHE_INCLUDE)
      APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_INSTALL_FILES $APACHE_TARGET"
      PHP_LIBS="-L. -lphp3"
      AC_MSG_RESULT(yes - Apache 1.2.x)
      STRONGHOLD=
      if test -f $withval/src/ap_config.h; then
        AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
      fi
    # For Apache 2.0.x
    elif test -f $withval/include/httpd.h &&
         test -f $withval/srclib/apr/include/apr_general.h ; then
      AC_MSG_ERROR([Use --with-apache2 with Apache 2.x!])
    # For Apache 1.3.x
    elif test -f $withval/src/main/httpd.h; then
      APACHE_HAS_REGEX=1
      APACHE_INCLUDE="-I$withval/src/main -I$withval/src/os/unix -I$withval/src/ap"
      APACHE_TARGET=$withval/src/modules/php4
      if test ! -d $APACHE_TARGET; then
        mkdir $APACHE_TARGET
      fi
      PHP_SELECT_SAPI(apache, static, sapi_apache.c mod_php4.c php_apache.c, $APACHE_INCLUDE)
      APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp4.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET; cp $srcdir/sapi/apache/apMakefile.tmpl $APACHE_TARGET/Makefile.tmpl; cp $srcdir/sapi/apache/apMakefile.libdir $APACHE_TARGET/Makefile.libdir"
      PHP_LIBS="-Lmodules/php4 -L../modules/php4 -L../../modules/php4 -lmodphp4"
      AC_MSG_RESULT(yes - Apache 1.3.x)
      STRONGHOLD=
      if test -f $withval/src/include/ap_config.h; then
        AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
      fi
      if test -f $withval/src/include/ap_compat.h; then
        AC_DEFINE(HAVE_AP_COMPAT_H,1,[ ])
        if test ! -f $withval/src/include/ap_config_auto.h; then
          AC_MSG_ERROR(Please run Apache\'s configure or src/Configure program once and try again)
        fi
      else
        if test -f $withval/src/include/compat.h; then
          AC_DEFINE(HAVE_OLD_COMPAT_H,1,[ ])
        fi
      fi
    # Also for Apache 1.3.x
    elif test -f $withval/src/include/httpd.h; then
      APACHE_HAS_REGEX=1
      APACHE_INCLUDE="-I$withval/src/include -I$withval/src/os/unix"
      APACHE_TARGET=$withval/src/modules/php4
      if test ! -d $APACHE_TARGET; then
        mkdir $APACHE_TARGET
      fi
      PHP_SELECT_SAPI(apache, static, sapi_apache.c mod_php4.c php_apache.c, $APACHE_INCLUDE)
      PHP_LIBS="-Lmodules/php4 -L../modules/php4 -L../../modules/php4 -lmodphp4"
      APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp4.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET; cp $srcdir/sapi/apache/apMakefile.tmpl $APACHE_TARGET/Makefile.tmpl; cp $srcdir/sapi/apache/apMakefile.libdir $APACHE_TARGET/Makefile.libdir"
      AC_MSG_RESULT(yes - Apache 1.3.x)
      STRONGHOLD=
      if test -f $withval/src/include/ap_config.h; then
        AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
      fi
      if test -f $withval/src/include/ap_compat.h; then
        AC_DEFINE(HAVE_AP_COMPAT_H,1,[ ])
        if test ! -f $withval/src/include/ap_config_auto.h; then
          AC_MSG_ERROR(Please run Apache\'s configure or src/Configure program once and try again)
        fi
      else
        if test -f $withval/src/include/compat.h; then
          AC_DEFINE(HAVE_OLD_COMPAT_H,1,[ ])
        fi
      fi
    # For StrongHold 2.2
    elif test -f $withval/apache/httpd.h; then
      APACHE_INCLUDE=-"I$withval/apache -I$withval/ssl/include"
      APACHE_TARGET=$withval/apache
      PHP_SELECT_SAPI(apache, static, sapi_apache.c mod_php4.c php_apache.c, $APACHE_INCLUDE)
      PHP_LIBS="-Lmodules/php4 -L../modules/php4 -L../../modules/php4 -lmodphp4"
      APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp4.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET"
      STRONGHOLD=-DSTRONGHOLD=1
      AC_MSG_RESULT(yes - StrongHold)
      if test -f $withval/apache/ap_config.h; then
        AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
      fi
      if test -f $withval/src/ap_compat.h; then
        AC_DEFINE(HAVE_AP_COMPAT_H,1,[ ])
        if test ! -f $withval/src/include/ap_config_auto.h; then
          AC_MSG_ERROR(Please run Apache\'s configure or src/Configure program once and try again)
        fi
      else
        if test -f $withval/src/compat.h; then
          AC_DEFINE(HAVE_OLD_COMPAT_H,1,[ ])
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

fi

if test "x$APXS" != "x" -a "`uname -sv`" = "AIX 4" -a "$GCC" != "yes"; then
  APXS_EXP=-bE:sapi/apache/mod_php4.exp
fi

PHP_SUBST(APXS_EXP)
PHP_SUBST(APACHE_INCLUDE)
PHP_SUBST(APACHE_TARGET)
PHP_SUBST(APXS)
PHP_SUBST(APXS_LDFLAGS)
PHP_SUBST(APACHE_INSTALL)
PHP_SUBST(STRONGHOLD)

AC_CACHE_CHECK([for member fd in BUFF *],ac_cv_php_fd_in_buff,[
  save=$CPPFLAGS
  CPPFLAGS="$CPPFLAGS $APACHE_INCLUDE"
  AC_TRY_COMPILE([#include <httpd.h>],[conn_rec *c; int fd = c->client->fd;],[
    ac_cv_php_fd_in_buff=yes],[ac_cv_php_fd_in_buff=no],[ac_cv_php_fd_in_buff=no])
  CPPFLAGS=$save
],[
  if test "$ac_cv_php_fd_in_buff" = "yes"; then
    AC_DEFINE(PHP_APACHE_HAVE_CLIENT_FD, 1, [ ])
  fi
])

  
AC_MSG_CHECKING(for mod_charset compatibility option)
AC_ARG_WITH(mod_charset,
[  --with-mod_charset      Enable transfer tables for mod_charset (Rus Apache).],
[
  AC_MSG_RESULT(yes)
  AC_DEFINE(USE_TRANSFER_TABLES,1,[ ])
],[
  AC_MSG_RESULT(no)
])

if test -n "$APACHE_MODULE"; then
  PHP_TARGET_RDYNAMIC
  $php_shtool mkdir -p sapi/apache
  PHP_OUTPUT(sapi/apache/libphp4.module)
fi

if test -n "$APACHE_INSTALL"; then
  INSTALL_IT=$APACHE_INSTALL
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
