dnl
dnl $Id$
dnl

PHP_ARG_WITH(apxs2filter,,
[  --with-apxs2filter[=FILE]   
                          EXPERIMENTAL: Build shared Apache 2.0 Filter module. FILE is the optional
                          pathname to the Apache apxs tool [apxs]], no, no)

AC_MSG_CHECKING([for Apache 2.0 filter-module support via DSO through APXS])

if test "$PHP_APXS2FILTER" != "no"; then
  if test "$PHP_APXS2FILTER" = "yes"; then
    APXS=apxs
    $APXS -q CFLAGS >/dev/null 2>&1
    if test "$?" != "0" && test -x /usr/sbin/apxs; then
      APXS=/usr/sbin/apxs
    fi
  else
    PHP_EXPAND_PATH($PHP_APXS2FILTER, APXS)
  fi

  $APXS -q CFLAGS >/dev/null 2>&1
  if test "$?" != "0"; then
    AC_MSG_RESULT()
    AC_MSG_RESULT()
    AC_MSG_RESULT([Sorry, I cannot run apxs.  Possible reasons follow:]) 
    AC_MSG_RESULT()
    AC_MSG_RESULT([1. Perl is not installed])
    AC_MSG_RESULT([2. apxs was not found. Try to pass the path using --with-apxs2filter=/path/to/apxs])
    AC_MSG_RESULT([3. Apache was not built using --enable-so (the apxs usage page is displayed)])
    AC_MSG_RESULT()
    AC_MSG_RESULT([The output of $APXS follows:])
    $APXS -q CFLAGS
    AC_MSG_ERROR([Aborting])
  fi 

  APXS_INCLUDEDIR=`$APXS -q INCLUDEDIR`
  APXS_BINDIR=`$APXS -q BINDIR`
  APXS_HTTPD=`$APXS -q SBINDIR`/`$APXS -q TARGET`
  APXS_CFLAGS=`$APXS -q CFLAGS`
  APU_BINDIR=`$APXS -q APU_BINDIR`
  APR_BINDIR=`$APXS -q APR_BINDIR`

  # Pick up ap[ru]-N-config if using httpd >=2.1
  APR_CONFIG=`$APXS -q APR_CONFIG 2>/dev/null ||
    echo $APR_BINDIR/apr-config`
  APU_CONFIG=`$APXS -q APU_CONFIG 2>/dev/null ||
    echo $APU_BINDIR/apu-config`

  APR_CFLAGS="`$APR_CONFIG --cppflags --includes`"
  APU_CFLAGS="`$APU_CONFIG --includes`"

  for flag in $APXS_CFLAGS; do
    case $flag in
    -D*) APACHE_CPPFLAGS="$APACHE_CPPFLAGS $flag";;
    esac
  done

  APACHE_CFLAGS="$APACHE_CPPFLAGS -I$APXS_INCLUDEDIR $APR_CFLAGS $APU_CFLAGS"

  # Test that we're trying to configure with apache 2.x
  PHP_AP_EXTRACT_VERSION($APXS_HTTPD)
  if test "$APACHE_VERSION" -le 2000000; then
    AC_MSG_ERROR([You have enabled Apache 2 support while your server is Apache 1.3.  Please use the appropriate switch --with-apxs (without the 2)])
  elif test "$APACHE_VERSION" -lt 2000040; then
    AC_MSG_ERROR([Please note that Apache version >= 2.0.40 is required])
  fi

  APXS_LIBEXECDIR='$(INSTALL_ROOT)'`$APXS -q LIBEXECDIR`
  if test -z `$APXS -q SYSCONFDIR`; then
    INSTALL_IT="\$(mkinstalldirs) '$APXS_LIBEXECDIR' && \
                 $APXS -S LIBEXECDIR='$APXS_LIBEXECDIR' \
                       -i -n php7"
  else
    APXS_SYSCONFDIR='$(INSTALL_ROOT)'`$APXS -q SYSCONFDIR`
    INSTALL_IT="\$(mkinstalldirs) '$APXS_LIBEXECDIR' && \
                \$(mkinstalldirs) '$APXS_SYSCONFDIR' && \
                 $APXS -S LIBEXECDIR='$APXS_LIBEXECDIR' \
                       -S SYSCONFDIR='$APXS_SYSCONFDIR' \
                       -i -a -n php7"
  fi

  case $host_alias in
  *aix*)
    EXTRA_LDFLAGS="$EXTRA_LDFLAGS -Wl,-brtl -Wl,-bI:$APXS_LIBEXECDIR/httpd.exp"
    PHP_SELECT_SAPI(apache2filter, shared, sapi_apache2.c apache_config.c php_functions.c, $APACHE_CFLAGS)
    INSTALL_IT="$INSTALL_IT $SAPI_LIBTOOL" 
    ;;
  *darwin*)
    dnl When using bundles on Darwin, we must resolve all symbols.  However,
    dnl the linker does not recursively look at the bundle loader and
    dnl pull in its dependencies.  Therefore, we must pull in the APR
    dnl and APR-util libraries.
    if test -x "$APR_CONFIG"; then
        MH_BUNDLE_FLAGS="`$APR_CONFIG --ldflags --link-ld --libs`"
    fi
    if test -x "$APU_CONFIG"; then
        MH_BUNDLE_FLAGS="`$APU_CONFIG --ldflags --link-ld --libs` $MH_BUNDLE_FLAGS"
    fi
    MH_BUNDLE_FLAGS="-bundle -bundle_loader $APXS_HTTPD $MH_BUNDLE_FLAGS"
    PHP_SUBST(MH_BUNDLE_FLAGS)
    PHP_SELECT_SAPI(apache2filter, bundle, sapi_apache2.c apache_config.c php_functions.c, $APACHE_CFLAGS)
    SAPI_SHARED=libs/libphp7.so
    INSTALL_IT="$INSTALL_IT $SAPI_SHARED"
    ;;
  *beos*)
    if test -f _APP_; then `rm _APP_`; fi
    `ln -s $APXS_BINDIR/httpd _APP_`
    EXTRA_LIBS="$EXTRA_LIBS _APP_"
    PHP_SELECT_SAPI(apache2filter, shared, sapi_apache2.c apache_config.c php_functions.c, $APACHE_CFLAGS)
    INSTALL_IT="$INSTALL_IT $SAPI_LIBTOOL"
    ;;
  *)
    PHP_SELECT_SAPI(apache2filter, shared, sapi_apache2.c apache_config.c php_functions.c, $APACHE_CFLAGS) 
    INSTALL_IT="$INSTALL_IT $SAPI_LIBTOOL"
    ;;
  esac

  if test "$APACHE_VERSION" -lt 2004001; then
    APXS_MPM=`$APXS -q MPM_NAME`
    if test "$APXS_MPM" != "prefork" && test "$APXS_MPM" != "peruser" && test "$APXS_MPM" != "itk"; then
      PHP_BUILD_THREAD_SAFE
    fi
  else
    APACHE_THREADED_MPM=`$APXS_HTTPD -V | grep 'threaded:.*yes'`
    if test -n "$APACHE_THREADED_MPM"; then
      PHP_BUILD_THREAD_SAFE
    fi
  fi
  AC_MSG_RESULT(yes)
  PHP_SUBST(APXS)
else
  AC_MSG_RESULT(no)
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
