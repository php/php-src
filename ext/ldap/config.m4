dnl config.m4 for extension ldap

AC_DEFUN([PHP_LDAP_CHECKS], [
  if test -f $1/include/ldap.h; then
    LDAP_DIR=$1
    LDAP_INCDIR=$1/include
    LDAP_LIBDIR=$1/$PHP_LIBDIR
  elif test -f $1/include/umich-ldap/ldap.h; then
    LDAP_DIR=$1
    LDAP_INCDIR=$1/include/umich-ldap
    LDAP_LIBDIR=$1/$PHP_LIBDIR
  elif test -f $1/ldap/public/ldap.h; then
    LDAP_DIR=$1
    LDAP_INCDIR=$1/ldap/public
    LDAP_LIBDIR=$1/$PHP_LIBDIR
  else

    dnl Find Oracle Instant Client RPM header location corresponding to the given lib path e.g. for --with-ldap=/usr/lib/oracle/12.1/client64/lib
    AC_CHECK_SIZEOF(long int, 4)
    if test "$ac_cv_sizeof_long_int" = "4"; then
      PHP_OCI8_IC_LIBDIR_SUFFIX=""
    else
      PHP_OCI8_IC_LIBDIR_SUFFIX=64
    fi
    OCISDKRPMINC=`echo "$1" | $SED -e 's!^/usr/lib/oracle/\(.*\)/client\('${PHP_OCI8_IC_LIBDIR_SUFFIX}'\)*/lib[/]*$!/usr/include/oracle/\1/client\2!'`

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

AC_DEFUN([PHP_LDAP_SASL_CHECKS], [
  if test "$1" = "yes"; then
    SEARCH_DIRS="/usr/local /usr"
  else
    SEARCH_DIRS=$1
  fi

  for i in $SEARCH_DIRS; do
    if test -f $i/include/sasl/sasl.h; then
      LDAP_SASL_DIR=$i
      AC_DEFINE(HAVE_LDAP_SASL_SASL_H,1,[ ])
      break
    elif test -f $i/include/sasl.h; then
      LDAP_SASL_DIR=$i
      AC_DEFINE(HAVE_LDAP_SASL_H,1,[ ])
      break
    fi
  done

  if test "$LDAP_SASL_DIR"; then
    LDAP_SASL_INCDIR=$LDAP_SASL_DIR/include
    LDAP_SASL_LIBDIR=$LDAP_SASL_DIR/$PHP_LIBDIR
  else
    AC_MSG_ERROR([sasl.h not found!])
  fi

  if test "$PHP_LDAP_SASL" = "yes"; then
    SASL_LIB="-lsasl2"
  else
    SASL_LIB="-L$LDAP_SASL_LIBDIR -lsasl2"
  fi

  PHP_CHECK_LIBRARY(sasl2, sasl_version,
  [
    PHP_ADD_INCLUDE($LDAP_SASL_INCDIR)
    PHP_ADD_LIBRARY_WITH_PATH(sasl2, $LDAP_SASL_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_LDAP_SASL, 1, [LDAP SASL support])
  ], [
    AC_MSG_ERROR([LDAP SASL check failed. Please check config.log for more information.])
  ], [
    $LDAP_SHARED_LIBADD $SASL_LIB
  ])
])

PHP_ARG_WITH(ldap,for LDAP support,
[  --with-ldap[=DIR]         Include LDAP support])

PHP_ARG_WITH(ldap-sasl,for LDAP Cyrus SASL support,
[  --with-ldap-sasl[=DIR]    LDAP: Include Cyrus SASL support], no, no)

if test "$PHP_LDAP" != "no"; then

  PHP_NEW_EXTENSION(ldap, ldap.c, $ext_shared,,-DLDAP_DEPRECATED=1)

  if test "$PHP_LDAP" = "yes"; then
    for i in /usr/local /usr; do
      PHP_LDAP_CHECKS($i)
    done
  else
    PHP_LDAP_CHECKS($PHP_LDAP)
  fi

  if test -z "$LDAP_DIR"; then
    AC_MSG_ERROR(Cannot find ldap.h)
  fi

  dnl The Linux version of the SDK need -lpthread
  dnl I have tested Solaris, and it doesn't, but others may.  Add
  dnl these here if necessary.   -RL
  dnl Is this really necessary? -Troels Arvin

  if test `uname` = "Linux"; then
    LDAP_PTHREAD=pthread
  else
    LDAP_PTHREAD=
  fi

  MACHINE_INCLUDES=$($CC -dumpmachine)

  if test -f $LDAP_LIBDIR/liblber.a || test -f $LDAP_LIBDIR/liblber.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/liblber.a || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/liblber.$SHLIB_SUFFIX_NAME; then
    PHP_ADD_LIBRARY_WITH_PATH(lber, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(ldap, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)

  elif test -f $LDAP_LIBDIR/libldap.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/libldap.$SHLIB_SUFFIX_NAME.3 || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libldap.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libldap.$SHLIB_SUFFIX_NAME.3 || test -f $LDAP_LIBDIR/libldap.3.dylib; then
    PHP_ADD_LIBRARY_WITH_PATH(ldap, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)

  elif test -f $LDAP_LIBDIR/libssldap50.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libssldap50.$SHLIB_SUFFIX_NAME; then
    if test -n "$LDAP_PTHREAD"; then
      PHP_ADD_LIBRARY($LDAP_PTHREAD)
    fi
    PHP_ADD_LIBRARY_WITH_PATH(nspr4, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(plc4, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(plds4, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(ssldap50, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(ldap50, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(prldap50, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(ssl3, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_NSLDAP,1,[ ])

  elif test -f $LDAP_LIBDIR/libldapssl41.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libldapssl41.$SHLIB_SUFFIX_NAME; then
    if test -n "$LDAP_PTHREAD"; then
      PHP_ADD_LIBRARY($LDAP_PTHREAD)
    fi
    PHP_ADD_LIBRARY_WITH_PATH(nspr3, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(plc3, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(plds3, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(ldapssl41, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_NSLDAP,1,[ ])

  elif test -f $LDAP_LIBDIR/libldapssl30.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libldapssl30.$SHLIB_SUFFIX_NAME; then
    if test -n "$LDAP_PTHREAD"; then
      PHP_ADD_LIBRARY($LDAP_PTHREAD)
    fi
    PHP_ADD_LIBRARY_WITH_PATH(ldapssl30, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_NSLDAP,1,[ ])

  elif test -f $LDAP_LIBDIR/libldap30.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libldap30.$SHLIB_SUFFIX_NAME; then
    if test -n "$LDAP_PTHREAD"; then
      PHP_ADD_LIBRARY($LDAP_PTHREAD)
    fi
    PHP_ADD_LIBRARY_WITH_PATH(ldap30, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_NSLDAP,1,[ ])

  elif test -f $LDAP_LIBDIR/libumich_ldap.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libumich_ldap.$SHLIB_SUFFIX_NAME; then
    PHP_ADD_LIBRARY_WITH_PATH(umich_lber, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    PHP_ADD_LIBRARY_WITH_PATH(umich_ldap, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)

  elif test -f $LDAP_LIBDIR/libclntsh.$SHLIB_SUFFIX_NAME.12.1 || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libclntsh.$SHLIB_SUFFIX_NAME.12.1; then
    PHP_ADD_LIBRARY_WITH_PATH(clntsh, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_ORALDAP,1,[ ])
    AC_DEFINE(HAVE_ORALDAP_12,1,[ ])

  elif test -f $LDAP_LIBDIR/libclntsh.$SHLIB_SUFFIX_NAME.11.1 || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libclntsh.$SHLIB_SUFFIX_NAME.11.1; then
    PHP_ADD_LIBRARY_WITH_PATH(clntsh, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
    AC_DEFINE(HAVE_ORALDAP,1,[ ])
    AC_DEFINE(HAVE_ORALDAP_11,1,[ ])

  elif test -f $LDAP_LIBDIR/libclntsh.$SHLIB_SUFFIX_NAME || test -f $LDAP_LIBDIR/$MACHINE_INCLUDES/libclntsh.$SHLIB_SUFFIX_NAME; then
     PHP_ADD_LIBRARY_WITH_PATH(clntsh, $LDAP_LIBDIR, LDAP_SHARED_LIBADD)
     AC_DEFINE(HAVE_ORALDAP,1,[ ])
     AC_DEFINE(HAVE_ORALDAP_10,1,[ ])

  else
    AC_MSG_ERROR(Cannot find ldap libraries in $LDAP_LIBDIR.)
  fi

  PHP_ADD_INCLUDE($LDAP_INCDIR)
  PHP_SUBST(LDAP_SHARED_LIBADD)
  AC_DEFINE(HAVE_LDAP,1,[ ])

  dnl Save original values
  _SAVE_CPPFLAGS=$CPPFLAGS
  _SAVE_LIBS=$LIBS
  CPPFLAGS="$CPPFLAGS -I$LDAP_INCDIR"
  LIBS="$LIBS $LDAP_SHARED_LIBADD"

  dnl Check for 3 arg ldap_set_rebind_proc
  AC_CACHE_CHECK([for 3 arg ldap_set_rebind_proc], ac_cv_3arg_setrebindproc,
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <ldap.h>]], [[ldap_set_rebind_proc(0,0,0)]])],
  [ac_cv_3arg_setrebindproc=yes], [ac_cv_3arg_setrebindproc=no])])
  if test "$ac_cv_3arg_setrebindproc" = yes; then
    AC_DEFINE(HAVE_3ARG_SETREBINDPROC,1,[Whether 3 arg set_rebind_proc()])
  fi

  dnl Solaris 2.8 claims to be 2004 API, but doesn't have
  dnl ldap_parse_reference() nor ldap_start_tls_s()
  AC_CHECK_FUNCS([ldap_parse_result ldap_parse_reference ldap_start_tls_s ldap_control_find ldap_parse_extended_result ldap_extended_operation ldap_extended_operation_s ldap_passwd ldap_whoami_s ldap_refresh_s])

  dnl
  dnl SASL check
  dnl
  if test "$PHP_LDAP_SASL" != "no"; then
    PHP_LDAP_SASL_CHECKS([$PHP_LDAP_SASL])
  fi

  dnl
  dnl Sanity check
  dnl
  AC_CHECK_FUNC(ldap_bind_s, [], [
    AC_MSG_ERROR([LDAP build check failed. Please check config.log for more information.])
  ])

  dnl Restore original values
  CPPFLAGS=$_SAVE_CPPFLAGS
  LIBS=$_SAVE_LIBS
fi
