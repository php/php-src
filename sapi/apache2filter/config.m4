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

  APXS_HTTPD=`$APXS -q SBINDIR`/`$APXS -q TARGET`

  # Test that we're trying to configure with apache 2.x
  PHP_AP_EXTRACT_VERSION($APXS_HTTPD)
  if test "$APACHE_VERSION" -le 2000000; then
    AC_MSG_ERROR([Use --with-apxs with Apache 1.3.x!])
  elif test "$APACHE_VERSION" -lt 2000035; then
    AC_MSG_ERROR([Apache version >= 2.0.35 required.])
  fi

  APXS_INCLUDEDIR=`$APXS -q INCLUDEDIR`
  APXS_CFLAGS=`$APXS -q CFLAGS`
  for flag in $APXS_CFLAGS; do
    case $flag in
    -D*) CPPFLAGS="$CPPFLAGS $flag";;
    esac
  done

  PHP_ADD_INCLUDE($APXS_INCLUDEDIR)
  PHP_SAPI=apache2filter
  INSTALL_IT="$APXS -i -a -n php4 $SAPI_LIBTOOL"
  PHP_BUILD_SHARED
  PHP_BUILD_THREAD_SAFE
  AC_MSG_RESULT(yes)
  case $host_alias in
  *aix*)
    APXS_SBINDIR=`$APXS -q SBINDIR`
    EXTRA_LDFLAGS="$EXTRA_LDFLAGS -Wl,-bI:$APXS_SBINDIR/httpd.exp"
    ;;
  esac
],[
  AC_MSG_RESULT(no)
])

PHP_SUBST(APXS)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
