PHP_ARG_WITH([snmp],
  [for SNMP support],
  [AS_HELP_STRING([[--with-snmp[=DIR]]],
    [Include SNMP support. Use PKG_CONFIG_PATH (or SNMP_CFLAGS and SNMP_LIBS)
    environment variables, or alternatively the optional DIR argument to
    customize where to look for the net-snmp-config utility of the NET-SNMP
    library.])])

if test "$PHP_SNMP" != "no"; then
  snmp_found=no
  AS_VAR_IF([PHP_SNMP], [yes],
    [PKG_CHECK_MODULES([SNMP], [netsnmp >= 5.3], [snmp_found=yes], [:])])

  AS_VAR_IF([snmp_found], [no], [
    AS_VAR_IF([PHP_SNMP], [yes],
      [AC_PATH_PROG([SNMP_CONFIG], [net-snmp-config],, [/usr/local/bin:$PATH])],
      [SNMP_CONFIG="$PHP_SNMP/bin/net-snmp-config"])

    AS_IF([test ! -x "$SNMP_CONFIG"],
      [AC_MSG_ERROR(m4_text_wrap([
        Could not find net-snmp-config binary. Please check your net-snmp
        installation.
      ]))])

    snmp_version=$($SNMP_CONFIG --version)
    AS_VERSION_COMPARE([$snmp_version], [5.3],
      [AC_MSG_ERROR(m4_text_wrap([
        Net-SNMP version 5.3 or greater required (detected $snmp_version).
      ]))])

    SNMP_PREFIX=$($SNMP_CONFIG --prefix)
    SNMP_CFLAGS="-I${SNMP_PREFIX}/include"
    SNMP_LIBS=$($SNMP_CONFIG --netsnmp-libs)
    SNMP_LIBS="$SNMP_LIBS $($SNMP_CONFIG --external-libs)"

    AS_IF([test -z "$SNMP_LIBS" || test -z "$SNMP_PREFIX"],
      [AC_MSG_ERROR(m4_text_wrap([
        Could not find the required paths. Please check your net-snmp
        installation.
      ]))])
  ])

  PHP_EVAL_INCLINE([$SNMP_CFLAGS])
  PHP_EVAL_LIBLINE([$SNMP_LIBS], [SNMP_SHARED_LIBADD])
  SNMP_LIBNAME=netsnmp

  dnl Test build.
  PHP_CHECK_LIBRARY([$SNMP_LIBNAME], [init_snmp],
    [AC_DEFINE([HAVE_SNMP], [1],
      [Define to 1 if the PHP extension 'snmp' is available.])],
    [AC_MSG_FAILURE([SNMP sanity check failed.])],
    [$SNMP_SHARED_LIBADD])

  dnl Check whether shutdown_snmp_logging() exists.
  PHP_CHECK_LIBRARY([$SNMP_LIBNAME], [shutdown_snmp_logging],
    [AC_DEFINE([HAVE_SHUTDOWN_SNMP_LOGGING], [1],
      [Define to 1 if SNMP library has the 'shutdown_snmp_logging' function.])],
    [],
    [$SNMP_SHARED_LIBADD])

  CFLAGS_SAVE=$CFLAGS
  LIBS_SAVE=$LIBS
  CFLAGS="$CFLAGS $SNMP_CFLAGS"
  LIBS="$LIBS $SNMP_LIBS"

  AC_CHECK_DECL([usmHMAC192SHA256AuthProtocol],
    [AC_DEFINE([HAVE_SNMP_SHA256], [1],
      [Define to 1 if SNMP library has the 'usmHMAC192SHA256AuthProtocol'
      array.])],
    [],
    [
      #include <net-snmp/net-snmp-config.h>
      #include <net-snmp/net-snmp-includes.h>
    ])

  AC_CHECK_DECL([usmHMAC384SHA512AuthProtocol],
    [AC_DEFINE([HAVE_SNMP_SHA512], [1],
      [Define to 1 if SNMP library has the 'usmHMAC384SHA512AuthProtocol'
      array.])],
    [],
    [
      #include <net-snmp/net-snmp-config.h>
      #include <net-snmp/net-snmp-includes.h>
    ])

  CFLAGS=$CFLAGS_SAVE
  LIBS=$LIBS_SAVE

  PHP_NEW_EXTENSION([snmp], [snmp.c], [$ext_shared])
  PHP_ADD_EXTENSION_DEP(snmp, spl)
  PHP_SUBST([SNMP_SHARED_LIBADD])
fi
