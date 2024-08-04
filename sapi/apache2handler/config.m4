PHP_ARG_WITH([apxs2],
  [whether to build Apache 2 handler module support via DSO through APXS],
  [AS_HELP_STRING([[--with-apxs2[=FILE]]],
    [Build shared Apache 2 handler module. FILE is the optional pathname to
    the Apache apxs tool [apxs]])],
  [no],
  [no])

if test "$PHP_APXS2" != "no"; then
  if test "$PHP_APXS2" = "yes"; then
    APXS=apxs
    $APXS -q CFLAGS >/dev/null 2>&1
    if test "$?" != "0" && test -x /usr/sbin/apxs; then
      APXS=/usr/sbin/apxs
    fi
  else
    PHP_EXPAND_PATH([$PHP_APXS2], [APXS])
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
    $APXS -q CFLAGS
    AC_MSG_ERROR([Aborting])
  fi

  APXS_INCLUDEDIR=`$APXS -q INCLUDEDIR`
  APXS_HTTPD=`$APXS -q SBINDIR`/`$APXS -q TARGET`
  AS_IF([test ! -x "$APXS_HTTPD"], [AC_MSG_ERROR(m4_normalize([
    $APXS_HTTPD executable not found. Please, install Apache HTTP Server
    command-line utility.
  ]))])

  APXS_CFLAGS=`$APXS -q CFLAGS`
  APU_BINDIR=`$APXS -q APU_BINDIR`
  APR_BINDIR=`$APXS -q APR_BINDIR`

  dnl Pick up ap[ru]-N-config.
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

  APACHE_CFLAGS="$APACHE_CPPFLAGS -I$APXS_INCLUDEDIR $APR_CFLAGS $APU_CFLAGS -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1"

  dnl Check Apache version.
  PHP_AP_EXTRACT_VERSION([$APXS_HTTPD])
  AS_VERSION_COMPARE([$APACHE_VERSION], [2004000],
    [AC_MSG_ERROR([Please note that Apache version >= 2.4 is required])])

  APXS_LIBEXECDIR='$(INSTALL_ROOT)'`$APXS -q LIBEXECDIR`
  if test -z `$APXS -q SYSCONFDIR`; then
    INSTALL_IT="\$(mkinstalldirs) '$APXS_LIBEXECDIR' && \
                 $APXS -S LIBEXECDIR='$APXS_LIBEXECDIR' \
                       -i -n php"
  else
    APXS_SYSCONFDIR='$(INSTALL_ROOT)'`$APXS -q SYSCONFDIR`
    INSTALL_IT="\$(mkinstalldirs) '$APXS_LIBEXECDIR' && \
                \$(mkinstalldirs) '$APXS_SYSCONFDIR' && \
                 $APXS -S LIBEXECDIR='$APXS_LIBEXECDIR' \
                       -S SYSCONFDIR='$APXS_SYSCONFDIR' \
                       -i -a -n php"
  fi

  LIBPHP_CFLAGS="-shared"
  PHP_SUBST([LIBPHP_CFLAGS])

  case $host_alias in
  *aix*)
    EXTRA_LDFLAGS="$EXTRA_LDFLAGS -Wl,-brtl -Wl,-bI:$APXS_LIBEXECDIR/httpd.exp"
    PHP_SELECT_SAPI([apache2handler],
      [shared],
      [mod_php.c sapi_apache2.c apache_config.c php_functions.c],
      [$APACHE_CFLAGS])
    INSTALL_IT="$INSTALL_IT $SAPI_LIBTOOL"
    ;;
  *darwin*)
    dnl When using bundles on Darwin, we must resolve all symbols. However, the
    dnl linker does not recursively look at the bundle loader and pull in its
    dnl dependencies. Therefore, we must pull in the APR and APR-util libraries.
    if test -x "$APR_CONFIG"; then
        MH_BUNDLE_FLAGS="`$APR_CONFIG --ldflags --link-ld --libs`"
    fi
    if test -x "$APU_CONFIG"; then
        MH_BUNDLE_FLAGS="`$APU_CONFIG --ldflags --link-ld --libs` $MH_BUNDLE_FLAGS"
    fi
    MH_BUNDLE_FLAGS="-bundle -bundle_loader $APXS_HTTPD $MH_BUNDLE_FLAGS"
    PHP_SUBST([MH_BUNDLE_FLAGS])
    PHP_SELECT_SAPI([apache2handler],
      [bundle],
      [mod_php.c sapi_apache2.c apache_config.c php_functions.c],
      [$APACHE_CFLAGS])
    SAPI_SHARED=libs/libphp.so
    INSTALL_IT="$INSTALL_IT $SAPI_SHARED"
    ;;
  *)
    PHP_SELECT_SAPI([apache2handler],
      [shared],
      [mod_php.c sapi_apache2.c apache_config.c php_functions.c],
      [$APACHE_CFLAGS])
    INSTALL_IT="$INSTALL_IT $SAPI_LIBTOOL"
    ;;
  esac

  AS_IF([$APXS_HTTPD -V 2>/dev/null | grep 'threaded:.*yes' >/dev/null 2>&1], [
    APACHE_THREADED_MPM=yes
    PHP_BUILD_THREAD_SAFE
  ], [APACHE_THREADED_MPM=no])

AC_CONFIG_COMMANDS([apache2handler], [AS_VAR_IF([enable_zts], [yes],,
  [AS_VAR_IF([APACHE_THREADED_MPM], [no],
    [AC_MSG_WARN([
+--------------------------------------------------------------------+
|                        *** WARNING ***                             |
|                                                                    |
| You have built PHP for Apache's current non-threaded MPM.          |
| If you change Apache to use a threaded MPM you must reconfigure    |
| PHP with --enable-zts                                              |
+--------------------------------------------------------------------+
  ])])])],
  [APACHE_THREADED_MPM="$APACHE_THREADED_MPM"; enable_zts="$enable_zts"])
fi
