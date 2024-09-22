AC_DEFUN([PHP_LDAP_CHECKS], [
  if test -f $1/include/ldap.h; then
    LDAP_DIR=$1
    LDAP_INCDIR=$1/include
    LDAP_LIBDIR=$1/$PHP_LIBDIR
  elif test -f $1/ldap/public/ldap.h; then
    LDAP_DIR=$1
    LDAP_INCDIR=$1/ldap/public
    LDAP_LIBDIR=$1/$PHP_LIBDIR
  else
    dnl Note that Oracle Instant Client lacks LDAP features implemented in PHP
    dnl ldap extension and cannot be built. Use OpenLDAP on Solaris.
    dnl See: https://github.com/php/php-src/issues/15051

    dnl Find Oracle Instant Client RPM header location corresponding to the
    dnl given lib path e.g. for --with-ldap=/usr/lib/oracle/12.1/client64/lib
    AC_CHECK_SIZEOF([long])
    AC_MSG_CHECKING([if we're at 64-bit platform])
    AS_IF([test "$ac_cv_sizeof_long" -eq 4],[
      AC_MSG_RESULT([no])
      PHP_OCI8_IC_LIBDIR_SUFFIX=""
    ],[
      AC_MSG_RESULT([yes])
      PHP_OCI8_IC_LIBDIR_SUFFIX=64
    ])

    OCISDKRPMINC=$(echo "$1" | $SED -e 's!^/usr/lib/oracle/\(.*\)/client\('${PHP_OCI8_IC_LIBDIR_SUFFIX}'\)*/lib[/]*$!/usr/include/oracle/\1/client\2!')

    dnl Check for Oracle Instant Client RPM install
    if test -f $OCISDKRPMINC/ldap.h; then
      LDAP_DIR=$1
      LDAP_INCDIR=$OCISDKRPMINC
      LDAP_LIBDIR=$1
    dnl Check for Oracle Instant Client ZIP install
    elif test -f $1/sdk/include/ldap.h; then
      LDAP_DIR=$1
      LDAP_INCDIR=$1/sdk/include
      LDAP_LIBDIR=$1
    fi
  fi
])

PHP_ARG_WITH([ldap],
  [for LDAP support],
  [AS_HELP_STRING([[--with-ldap[=DIR]]],
    [Include LDAP support])])

PHP_ARG_WITH([ldap-sasl],
  [whether to build with LDAP Cyrus SASL support],
  [AS_HELP_STRING([--with-ldap-sasl],
    [LDAP: Build with Cyrus SASL support])],
  [no],
  [no])

if test "$PHP_LDAP" != "no"; then
  PHP_NEW_EXTENSION([ldap],
    [ldap.c],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])

  AS_VAR_IF([PHP_LDAP], [yes], [
    for i in /usr/local /usr; do
      PHP_LDAP_CHECKS([$i])
    done
  ], [PHP_LDAP_CHECKS([$PHP_LDAP])])

  AS_VAR_IF([LDAP_DIR],, [AC_MSG_ERROR([Cannot find ldap.h])])

  dnl -pc removal is a hack for clang
  MACHINE_INCLUDES=$($CC -dumpmachine | $SED 's/-pc//')

  AH_TEMPLATE([HAVE_ORALDAP],
    [Define to 1 if the ldap extension uses the Oracle Instant Client.])

  if test -f $LDAP_LIBDIR/liblber.a || test -f $LDAP_LIBDIR/liblber.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/liblber.a || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/liblber.$SHLIB_SUFFIX_NAME; then
    PHP_ADD_LIBRARY_WITH_PATH([lber], [$LDAP_LIBDIR], [LDAP_SHARED_LIBADD])
    PHP_ADD_LIBRARY_WITH_PATH([ldap], [$LDAP_LIBDIR], [LDAP_SHARED_LIBADD])

  elif test -f $LDAP_LIBDIR/libldap.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/libldap.$SHLIB_SUFFIX_NAME.3 || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libldap.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libldap.$SHLIB_SUFFIX_NAME.3 || test -f $LDAP_LIBDIR/libldap.3.dylib; then
    PHP_ADD_LIBRARY_WITH_PATH([ldap], [$LDAP_LIBDIR], [LDAP_SHARED_LIBADD])

  elif test -f $LDAP_LIBDIR/libclntsh.$SHLIB_SUFFIX_NAME.12.1 || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libclntsh.$SHLIB_SUFFIX_NAME.12.1; then
    PHP_ADD_LIBRARY_WITH_PATH([clntsh], [$LDAP_LIBDIR], [LDAP_SHARED_LIBADD])
    AC_DEFINE([HAVE_ORALDAP], [1])

  elif test -f $LDAP_LIBDIR/libclntsh.$SHLIB_SUFFIX_NAME.11.1 || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libclntsh.$SHLIB_SUFFIX_NAME.11.1; then
    PHP_ADD_LIBRARY_WITH_PATH([clntsh], [$LDAP_LIBDIR], [LDAP_SHARED_LIBADD])
    AC_DEFINE([HAVE_ORALDAP], [1])

  elif test -f $LDAP_LIBDIR/libclntsh.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libclntsh.$SHLIB_SUFFIX_NAME; then
     PHP_ADD_LIBRARY_WITH_PATH([clntsh], [$LDAP_LIBDIR], [LDAP_SHARED_LIBADD])
     AC_DEFINE([HAVE_ORALDAP], [1])

  else
    AC_MSG_ERROR([Cannot find ldap libraries in $LDAP_LIBDIR.])
  fi

  PHP_ADD_INCLUDE([$LDAP_INCDIR])
  PHP_SUBST([LDAP_SHARED_LIBADD])
  AC_DEFINE([HAVE_LDAP], [1],
    [Define to 1 if the PHP extension 'ldap' is available.])

  dnl Save original values
  _SAVE_CPPFLAGS=$CPPFLAGS
  _SAVE_LIBS=$LIBS
  CPPFLAGS="$CPPFLAGS -I$LDAP_INCDIR"
  LIBS="$LIBS $LDAP_SHARED_LIBADD"

  dnl Check for 3 arg ldap_set_rebind_proc
  AC_CACHE_CHECK([for 3 arg ldap_set_rebind_proc],
    [php_cv_have_3arg_setrebindproc],
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include <ldap.h>],
    [ldap_set_rebind_proc(0,0,0)])],
    [php_cv_have_3arg_setrebindproc=yes],
    [php_cv_have_3arg_setrebindproc=no])])
  AS_VAR_IF([php_cv_have_3arg_setrebindproc], [yes],
    [AC_DEFINE([HAVE_3ARG_SETREBINDPROC], [1],
      [Define to 1 if 'ldap_set_rebind_proc' has 3 arguments.])])

  dnl Solaris 2.8 claims to be 2004 API, but doesn't have ldap_parse_reference()
  dnl nor ldap_start_tls_s()
  AC_CHECK_FUNCS(m4_normalize([
    ldap_control_find
    ldap_extended_operation
    ldap_extended_operation_s
    ldap_parse_extended_result
    ldap_parse_reference
    ldap_parse_result
    ldap_passwd
    ldap_refresh_s
    ldap_start_tls_s
    ldap_whoami_s
  ]))

  dnl SASL check
  AS_VAR_IF([PHP_LDAP_SASL], [no],, [
    PKG_CHECK_MODULES([SASL], [libsasl2])
    PHP_EVAL_INCLINE([$SASL_CFLAGS])
    PHP_EVAL_LIBLINE([$SASL_LIBS], [LDAP_SHARED_LIBADD])
    AC_DEFINE([HAVE_LDAP_SASL], [1],
      [Define to 1 if the ldap extension has SASL support enabled.])
  ])

  dnl Sanity check
  AC_CHECK_FUNC([ldap_sasl_bind_s],,
    [AC_CHECK_FUNC([ldap_simple_bind_s],,
      [AC_MSG_ERROR([LDAP library build check failed.])])])

  dnl Restore original values
  CPPFLAGS=$_SAVE_CPPFLAGS
  LIBS=$_SAVE_LIBS
fi
