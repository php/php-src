dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for Apache 2.0 module support via DSO through APXS)
AC_ARG_WITH(apxs2,
[  --with-apxs2[=FILE]     EXPERIMENTAL: Build shared Apache 2.0 module. FILE is the optional
                          pathname to the Apache apxs tool; defaults to "apxs".],[
  if test "$withval" = "yes"; then
    APXS=apxs
    $APXS -q CFLAGS >/dev/null 2>&1
    if test "$?" != "0" && test -x /usr/sbin/apxs; then
      APXS=/usr/sbin/apxs
    fi
  else
    PHP_EXPAND_PATH($withval, APXS)
  fi

  $APXS -q CFLAGS >/dev/null 2>&1
  if test "$?" != "0"; then
    AC_MSG_RESULT()
    AC_MSG_RESULT()
    AC_MSG_RESULT([Sorry, I cannot run apxs.  Possible reasons follow:]) 
    AC_MSG_RESULT()
    AC_MSG_RESULT([1. Perl is not installed])
    AC_MSG_RESULT([2. apxs was not found. Try to pass the path using --with-apxs2=/path/to/apxs])
    AC_MSG_RESULT([3. Apache was not built using --enable-so (the apxs usage page is displayed)])
    AC_MSG_RESULT()
    AC_MSG_RESULT([The output of $APXS follows:])
    $APXS
    AC_MSG_ERROR([Aborting])
  fi 

  APXS_INCLUDEDIR=`$APXS -q INCLUDEDIR`
  APXS_HTTPD=`$APXS -q SBINDIR`/`$APXS -q TARGET`
  APXS_CFLAGS=`$APXS -q CFLAGS`
  APXS_MPM=`$APXS -q MPM_NAME`

  for flag in $APXS_CFLAGS; do
    case $flag in
    -D*) CPPFLAGS="$CPPFLAGS $flag";;
    esac
  done

  # Test that we're trying to configure with apache 2.x
  PHP_AP_EXTRACT_VERSION($APXS_HTTPD)
  if test "$APACHE_VERSION" -le 2000000; then
    AC_MSG_ERROR([You have enabled Apache 2 support while your server is Apache 1.3.  Please use the appropiate switch --with-apxs (without the 2)])
  elif test "$APACHE_VERSION" -lt 2000040; then
    AC_MSG_ERROR([Please note that Apache version >= 2.0.40 is required.])
  fi

  APXS_LIBEXECDIR='$(INSTALL_ROOT)'`$APXS -q LIBEXECDIR`
  if test -z `$APXS -q SYSCONFDIR`; then
    optarg=
  else
    optarg=-a
  fi

  INSTALL_IT='$(mkinstalldirs) '"$APXS_LIBEXECDIR && $APXS -S LIBEXECDIR='$APXS_LIBEXECDIR' -i ${optarg} -n php4"

  case $host_alias in
  *aix*)
    EXTRA_LDFLAGS="$EXTRA_LDFLAGS -Wl,-brtl -Wl,-bI:$APXS_LIBEXECDIR/httpd.exp"
    PHP_SELECT_SAPI(apache2filter, shared, sapi_apache2.c apache_config.c php_functions.c)
    INSTALL_IT="$INSTALL_IT $SAPI_LIBTOOL" 
    ;;
  *darwin*)
    dnl When using bundles on Darwin, we must resolve all symbols.  However,
    dnl the linker does not recursively look at the bundle loader and
    dnl pull in its dependencies.  Therefore, we must pull in the APR
    dnl and APR-util libraries.
    APXS_BINDIR=`$APXS -q BINDIR`
    if test -f $APXS_BINDIR/apr-config; then
        MH_BUNDLE_FLAGS="`$APXS_BINDIR/apr-config --ldflags --link-ld --libs`"
    fi
    if test -f $APXS_BINDIR/apu-config; then
        MH_BUNDLE_FLAGS="`$APXS_BINDIR/apu-config --ldflags --link-ld --libs` $MH_BUNDLE_FLAGS"
    fi
    MH_BUNDLE_FLAGS="-bundle -bundle_loader $APXS_HTTPD $MH_BUNDLE_FLAGS"
    PHP_SUBST(MH_BUNDLE_FLAGS)
    PHP_SELECT_SAPI(apache2filter, bundle, sapi_apache2.c apache_config.c php_functions.c)
    SAPI_SHARED=libs/libphp4.so
    INSTALL_IT="$INSTALL_IT $SAPI_SHARED"
    ;;
  *beos*)
    APXS_BINDIR=`$APXS -q BINDIR`
    if test -f _APP_; then `rm _APP_`; fi
    `ln -s $APXS_BINDIR/httpd _APP_`
    EXTRA_LIBS="$EXTRA_LIBS _APP_"
    PHP_SELECT_SAPI(apache2filter, shared, sapi_apache2.c apache_config.c php_functions.c)
    INSTALL_IT="$INSTALL_IT $SAPI_LIBTOOL" 
    ;;
  *)
    PHP_SELECT_SAPI(apache2filter, shared, sapi_apache2.c apache_config.c php_functions.c) 
    INSTALL_IT="$INSTALL_IT $SAPI_LIBTOOL"
    ;;
  esac

  PHP_ADD_INCLUDE($APXS_INCLUDEDIR)
  if test "$APXS_MPM" != "prefork"; then
    PHP_BUILD_THREAD_SAFE
  fi
  AC_MSG_RESULT(yes)
],[
  AC_MSG_RESULT(no)
])

PHP_SUBST(APXS)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
