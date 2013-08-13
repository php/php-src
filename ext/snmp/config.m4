dnl
dnl $Id$
dnl

PHP_ARG_WITH(snmp,for SNMP support,
[  --with-snmp[=DIR]       Include SNMP support])

PHP_ARG_WITH(openssl-dir,OpenSSL dir for SNMP,
[  --with-openssl-dir[=DIR]  SNMP: openssl install prefix], no, no)

if test "$PHP_SNMP" != "no"; then

  if test "$PHP_SNMP" = "yes"; then
    AC_PATH_PROG(SNMP_CONFIG,net-snmp-config,,[/usr/local/bin:$PATH])
  else
    SNMP_CONFIG="$PHP_SNMP/bin/net-snmp-config"
  fi

  if test -x "$SNMP_CONFIG"; then
    SNMP_LIBS=`$SNMP_CONFIG --netsnmp-libs`
    SNMP_LIBS="$SNMP_LIBS `$SNMP_CONFIG --external-libs`"
    SNMP_PREFIX=`$SNMP_CONFIG --prefix`
    snmp_full_version=`$SNMP_CONFIG --version`
    ac_IFS=$IFS
    IFS="."
    set $snmp_full_version
    IFS=$ac_IFS
    SNMP_VERSION=`expr [$]1 \* 1000 + [$]2`
    if test "$SNMP_VERSION" -ge "5003"; then
      if test -n "$SNMP_LIBS" && test -n "$SNMP_PREFIX"; then
        PHP_ADD_INCLUDE(${SNMP_PREFIX}/include)
        PHP_EVAL_LIBLINE($SNMP_LIBS, SNMP_SHARED_LIBADD)
        SNMP_LIBNAME=netsnmp
      else
        AC_MSG_ERROR([Could not find the required paths. Please check your net-snmp installation.])
      fi
    else
      AC_MSG_ERROR([Net-SNMP version 5.3 or greater reqired (detected $snmp_full_version).])
    fi
  else 
    AC_MSG_ERROR([Could not find net-snmp-config binary. Please check your net-snmp installation.])
  fi

  dnl Test build.
  PHP_CHECK_LIBRARY($SNMP_LIBNAME, init_snmp,
  [
    AC_DEFINE(HAVE_SNMP,1,[ ])
  ], [
    AC_MSG_ERROR([SNMP sanity check failed. Please check config.log for more information.])
  ], [
    $SNMP_SHARED_LIBADD
  ])

  dnl Check whether shutdown_snmp_logging() exists.
  PHP_CHECK_LIBRARY($SNMP_LIBNAME, shutdown_snmp_logging,
  [
    AC_DEFINE(HAVE_SHUTDOWN_SNMP_LOGGING, 1, [ ])
  ], [], [
    $SNMP_SHARED_LIBADD
  ])

  PHP_NEW_EXTENSION(snmp, snmp.c, $ext_shared)
  PHP_SUBST(SNMP_SHARED_LIBADD)
fi
