dnl
dnl $Id$
dnl
AC_DEFUN([PHP_APACHE_FD_CHECK], [
AC_CACHE_CHECK([for member fd in BUFF *],ac_cv_php_fd_in_buff,[
  save=$CPPFLAGS
  if test -n "$APXS_INCLUDEDIR"; then
    CPPFLAGS="$CPPFLAGS -I$APXS_INCLUDEDIR"
  else
    CPPFLAGS="$CPPFLAGS $APACHE_INCLUDE"
  fi
  AC_TRY_COMPILE([#include <httpd.h>],[conn_rec *c; int fd = c->client->fd;],[
    ac_cv_php_fd_in_buff=yes],[ac_cv_php_fd_in_buff=no],[ac_cv_php_fd_in_buff=no])
  CPPFLAGS=$save
])
if test "$ac_cv_php_fd_in_buff" = "yes"; then
  AC_DEFINE(PHP_APACHE_HAVE_CLIENT_FD,1,[ ])
fi
])

dnl Apache 1.x shared module
PHP_ARG_WITH(apxs,,
[  --with-apxs[=FILE]      Build shared Apache 1.x module. FILE is the optional
                          pathname to the Apache apxs tool [apxs]], no, no)

AC_MSG_CHECKING([for Apache 1.x module support via DSO through APXS])

if test "$PHP_APXS" != "no"; then
  if test "$PHP_APXS" = "yes"; then
    APXS=apxs
    $APXS -q CFLAGS >/dev/null 2>&1
    if test "$?" != "0" && test -x /usr/sbin/apxs; then #SUSE 6.x 
      APXS=/usr/sbin/apxs
    fi
  else
    PHP_EXPAND_PATH($PHP_APXS, APXS)
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
    $APXS -q CFLAGS
    AC_MSG_ERROR([Aborting]) 
  fi 

  APXS_LDFLAGS="@SYBASE_LFLAGS@ @SYBASE_LIBS@ @SYBASE_CT_LFLAGS@ @SYBASE_CT_LIBS@"
  APXS_INCLUDEDIR=`$APXS -q INCLUDEDIR`
  APXS_CFLAGS=`$APXS -q CFLAGS`
  APXS_HTTPD=`$APXS -q SBINDIR`/`$APXS -q TARGET`
  APACHE_INCLUDE=-I$APXS_INCLUDEDIR

  # Test that we're trying to configure with apache 1.x
  PHP_AP_EXTRACT_VERSION($APXS_HTTPD)
  if test "$APACHE_VERSION" -ge 2000000; then
    AC_MSG_ERROR([You have enabled Apache 1.3 support while your server is Apache 2.  Please use the appropiate switch --with-apxs2]) 
  fi

  for flag in $APXS_CFLAGS; do
    case $flag in
    -D*) APACHE_CPPFLAGS="$APACHE_CPPFLAGS $flag";;
    esac
  done

  case $host_alias in
  *aix*)
    APXS_LIBEXECDIR=`$APXS -q LIBEXECDIR`
    EXTRA_LDFLAGS="$EXTRA_LDFLAGS -Wl,-brtl -Wl,-bI:$APXS_LIBEXECDIR/httpd.exp"
    PHP_AIX_LDFLAGS="-Wl,-brtl"
    build_type=shared
    ;;
  *darwin*)
    MH_BUNDLE_FLAGS="-dynamic -twolevel_namespace -bundle -bundle_loader $APXS_HTTPD"
    PHP_SUBST(MH_BUNDLE_FLAGS)
    SAPI_SHARED=libs/libphp5.so
    build_type=bundle
    ;;
  *)
    build_type=shared
    ;;
  esac

  PHP_SELECT_SAPI(apache, $build_type, sapi_apache.c mod_php5.c php_apache.c, $APACHE_CPPFLAGS -I$APXS_INCLUDEDIR)

  # Test whether apxs support -S option
  $APXS -q -S CFLAGS="$APXS_CFLAGS" CFLAGS >/dev/null 2>&1

  if test "$?" != "0"; then
    APACHE_INSTALL="$APXS -i -a -n php5 $SAPI_SHARED" # Old apxs does not have -S option
  else 
    APXS_LIBEXECDIR='$(INSTALL_ROOT)'`$APXS -q LIBEXECDIR`
    if test -z `$APXS -q SYSCONFDIR`; then
      APACHE_INSTALL="\$(mkinstalldirs) '$APXS_LIBEXECDIR' && \
                       $APXS -S LIBEXECDIR='$APXS_LIBEXECDIR' \
                             -i -n php5 $SAPI_SHARED"
    else
      APXS_SYSCONFDIR='$(INSTALL_ROOT)'`$APXS -q SYSCONFDIR`
      APACHE_INSTALL="\$(mkinstalldirs) '$APXS_LIBEXECDIR' && \
                      \$(mkinstalldirs) '$APXS_SYSCONFDIR' && \
                       $APXS -S LIBEXECDIR='$APXS_LIBEXECDIR' \
                             -S SYSCONFDIR='$APXS_SYSCONFDIR' \
                             -i -a -n php5 $SAPI_SHARED"
    fi
  fi

  if test -z "`$APXS -q LD_SHLIB`" || test "`$APXS -q LIBEXECDIR`" = "modules"; then
    PHP_APXS_BROKEN=yes
  fi
  STRONGHOLD=
  AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
  AC_DEFINE(HAVE_AP_COMPAT_H,1,[ ])
  AC_DEFINE(HAVE_APACHE,1,[ ])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

dnl Apache 1.x static module
PHP_ARG_WITH(apache,,
[  --with-apache[=DIR]     Build Apache 1.x module. DIR is the top-level Apache
                          build directory [/usr/local/apache]], no, no)

AC_MSG_CHECKING([for Apache 1.x module support])

if test "$PHP_SAPI" != "apache" && test "$PHP_APACHE" != "no"; then
  
  if test "$PHP_APACHE" = "yes"; then
    # Apache's default directory
    PHP_APACHE=/usr/local/apache
  fi

  APACHE_INSTALL_FILES="\$(srcdir)/sapi/apache/mod_php5.* sapi/apache/libphp5.module"

  AC_DEFINE(HAVE_APACHE,1,[ ])
  APACHE_MODULE=yes
  PHP_EXPAND_PATH($PHP_APACHE, PHP_APACHE)
  # For Apache 1.2.x
  if test -f $PHP_APACHE/src/httpd.h; then 
    APACHE_INCLUDE=-I$PHP_APACHE/src
    APACHE_TARGET=$PHP_APACHE/src
    PHP_SELECT_SAPI(apache, static, sapi_apache.c mod_php5.c php_apache.c, $APACHE_INCLUDE)
    APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_INSTALL_FILES $APACHE_TARGET"
    PHP_LIBS="-L. -lphp3"
    AC_MSG_RESULT([yes - Apache 1.2.x])
    STRONGHOLD=
    if test -f $PHP_APACHE/src/ap_config.h; then
      AC_DEFINE(HAVE_AP_CONFIG_H,1,[ ])
    fi
  # For Apache 2.0.x
  elif test -f $PHP_APACHE/include/httpd.h && test -f $PHP_APACHE/srclib/apr/include/apr_general.h ; then
    AC_MSG_ERROR([Use --with-apxs2 with Apache 2.x!])
  # For Apache 1.3.x
  elif test -f $PHP_APACHE/src/main/httpd.h; then
    APACHE_HAS_REGEX=1
    APACHE_INCLUDE="-I$PHP_APACHE/src/main -I$PHP_APACHE/src/os/unix -I$PHP_APACHE/src/ap"
    APACHE_TARGET=$PHP_APACHE/src/modules/php5
    if test ! -d $APACHE_TARGET; then
      mkdir $APACHE_TARGET
    fi
    PHP_SELECT_SAPI(apache, static, sapi_apache.c mod_php5.c php_apache.c, $APACHE_INCLUDE)
    APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp5.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET; cp $srcdir/sapi/apache/apMakefile.tmpl $APACHE_TARGET/Makefile.tmpl; cp $srcdir/sapi/apache/apMakefile.libdir $APACHE_TARGET/Makefile.libdir"
    PHP_LIBS="-Lmodules/php5 -L../modules/php5 -L../../modules/php5 -lmodphp5"
    AC_MSG_RESULT([yes - Apache 1.3.x])
    STRONGHOLD=
    if test -f $PHP_APACHE/src/include/ap_config.h; then
      AC_DEFINE(HAVE_AP_CONFIG_H, 1, [ ])
    fi
    if test -f $PHP_APACHE/src/include/ap_compat.h; then
      AC_DEFINE(HAVE_AP_COMPAT_H, 1, [ ])
      if test ! -f $PHP_APACHE/src/include/ap_config_auto.h; then
        AC_MSG_ERROR([Please run Apache\'s configure or src/Configure program once and try again])
      fi
    elif test -f $PHP_APACHE/src/include/compat.h; then
      AC_DEFINE(HAVE_OLD_COMPAT_H, 1, [ ])
    fi
  # Also for Apache 1.3.x
  elif test -f $PHP_APACHE/src/include/httpd.h; then
    APACHE_HAS_REGEX=1
    APACHE_INCLUDE="-I$PHP_APACHE/src/include -I$PHP_APACHE/src/os/unix"
    APACHE_TARGET=$PHP_APACHE/src/modules/php5
    if test ! -d $APACHE_TARGET; then
      mkdir $APACHE_TARGET
    fi
    PHP_SELECT_SAPI(apache, static, sapi_apache.c mod_php5.c php_apache.c, $APACHE_INCLUDE)
    PHP_LIBS="-Lmodules/php5 -L../modules/php5 -L../../modules/php5 -lmodphp5"
    APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp5.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET; cp $srcdir/sapi/apache/apMakefile.tmpl $APACHE_TARGET/Makefile.tmpl; cp $srcdir/sapi/apache/apMakefile.libdir $APACHE_TARGET/Makefile.libdir"
    AC_MSG_RESULT([yes - Apache 1.3.x])
    STRONGHOLD=
    if test -f $PHP_APACHE/src/include/ap_config.h; then
      AC_DEFINE(HAVE_AP_CONFIG_H, 1, [ ])
    fi
    if test -f $PHP_APACHE/src/include/ap_compat.h; then
      AC_DEFINE(HAVE_AP_COMPAT_H, 1, [ ])
      if test ! -f $PHP_APACHE/src/include/ap_config_auto.h; then
        AC_MSG_ERROR([Please run Apache\'s configure or src/Configure program once and try again])
      fi
    elif test -f $PHP_APACHE/src/include/compat.h; then
      AC_DEFINE(HAVE_OLD_COMPAT_H, 1, [ ])
    fi
  # For StrongHold 2.2
  elif test -f $PHP_APACHE/apache/httpd.h; then
    APACHE_INCLUDE="-I$PHP_APACHE/apache -I$PHP_APACHE/ssl/include"
    APACHE_TARGET=$PHP_APACHE/apache
    PHP_SELECT_SAPI(apache, static, sapi_apache.c mod_php5.c php_apache.c, $APACHE_INCLUDE)
    PHP_LIBS="-Lmodules/php5 -L../modules/php5 -L../../modules/php5 -lmodphp5"
    APACHE_INSTALL="mkdir -p $APACHE_TARGET; cp $SAPI_STATIC $APACHE_TARGET/libmodphp5.a; cp $APACHE_INSTALL_FILES $APACHE_TARGET"
    STRONGHOLD=-DSTRONGHOLD=1
    AC_MSG_RESULT([yes - StrongHold])
    if test -f $PHP_APACHE/apache/ap_config.h; then
      AC_DEFINE(HAVE_AP_CONFIG_H, 1, [ ])
    fi
    if test -f $PHP_APACHE/src/ap_compat.h; then
      AC_DEFINE(HAVE_AP_COMPAT_H, 1, [ ])
      if test ! -f $PHP_APACHE/src/include/ap_config_auto.h; then
        AC_MSG_ERROR([Please run Apache\'s configure or src/Configure program once and try again])
      fi
    elif test -f $PHP_APACHE/src/compat.h; then
      AC_DEFINE(HAVE_OLD_COMPAT_H, 1, [ ])
    fi
  else
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([Invalid Apache directory - unable to find httpd.h under $PHP_APACHE])
  fi
else
  AC_MSG_RESULT(no)
fi

# compatibility
if test -z "$enable_mod_charset" && test "$with_mod_charset"; then
  enable_mod_charset=$with_mod_charset
fi
  
PHP_ARG_ENABLE(mod-charset, whether to enable Apache charset compatibility option,
[  --enable-mod-charset      APACHE: Enable transfer tables for mod_charset (Rus Apache)], no, no)

if test "$PHP_MOD_CHARSET" = "yes"; then
  AC_DEFINE(USE_TRANSFER_TABLES, 1, [ ])
fi

dnl Build as static module
if test "$APACHE_MODULE" = "yes"; then
  PHP_TARGET_RDYNAMIC
  $php_shtool mkdir -p sapi/apache
  PHP_OUTPUT(sapi/apache/libphp5.module)
fi

dnl General
if test -n "$APACHE_INSTALL"; then
  if test "x$APXS" != "x" -a "`uname -sv`" = "AIX 4" -a "$GCC" != "yes"; then
    APXS_EXP=-bE:sapi/apache/mod_php5.exp
  fi

  PHP_APACHE_FD_CHECK
  INSTALL_IT=$APACHE_INSTALL

  PHP_SUBST(APXS_EXP)
  PHP_SUBST(APACHE_INCLUDE)
  PHP_SUBST(APACHE_TARGET)
  PHP_SUBST(APXS)
  PHP_SUBST(APXS_LDFLAGS)
  PHP_SUBST(APACHE_INSTALL)
  PHP_SUBST(STRONGHOLD)
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
