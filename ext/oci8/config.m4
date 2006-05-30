dnl
dnl $Id$
dnl

AC_DEFUN([PHP_OCI_IF_DEFINED],[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=$3
  AC_EGREP_CPP(yes,[
#include <oci.h>
#if defined($1)
    yes
#endif
  ],[
    CPPFLAGS=$old_CPPFLAGS
    $2
  ],[
    CPPFLAGS=$old_CPPFLAGS
  ])
])

AC_DEFUN([AC_OCI8_VERSION],[
  AC_MSG_CHECKING([Oracle version])
  if test -s "$OCI8_DIR/orainst/unix.rgs"; then
    OCI8_VERSION=`grep '"ocommon"' $OCI8_DIR/orainst/unix.rgs | sed 's/[ ][ ]*/:/g' | cut -d: -f 6 | cut -c 2-4`
    test -z "$OCI8_VERSION" && OCI8_VERSION=7.3
  elif test -f $OCI8_DIR/lib/libclntsh.$SHLIB_SUFFIX_NAME.10.1; then
    OCI8_VERSION=10.1    
  elif test -f $OCI8_DIR/lib/libclntsh.$SHLIB_SUFFIX_NAME.9.0; then
    OCI8_VERSION=9.0
  elif test -f $OCI8_DIR/lib/libclntsh.$SHLIB_SUFFIX_NAME.8.0; then
    OCI8_VERSION=8.1
  elif test -f $OCI8_DIR/lib/libclntsh.$SHLIB_SUFFIX_NAME.1.0; then
    OCI8_VERSION=8.0
  elif test -f $OCI8_DIR/lib/libclntsh.a; then 
    if test -f $OCI8_DIR/lib/libcore4.a; then 
      OCI8_VERSION=8.0
    else
      OCI8_VERSION=8.1
    fi
  else
    AC_MSG_ERROR([Oracle (OCI8) required libraries not found])
  fi
  AC_MSG_RESULT($OCI8_VERSION)
])                                                                                                                                                                

AC_DEFUN([AC_OCI8IC_VERSION],[
  AC_MSG_CHECKING([Oracle Instant Client version])
  if test -f $PHP_OCI8_INSTANT_CLIENT/libnnz10.$SHLIB_SUFFIX_NAME; then
    if test -f $PHP_OCI8_INSTANT_CLIENT/libclntsh.$SHLIB_SUFFIX_NAME.10.1; then
      if test ! -f $PHP_OCI8_INSTANT_CLIENT/libclntsh.$SHLIB_SUFFIX_NAME; then
        AC_MSG_ERROR([Link from $PHP_OCI8_INSTANT_CLIENT/libclntsh.$SHLIB_SUFFIX_NAME to libclntsh.$SHLIB_SUFFIX_NAME.10.1 not found])
      fi
      OCI8_VERSION=10.1
    else
      AC_MSG_ERROR([Oracle Instant Client library version not supported])
    fi
  else
    AC_MSG_ERROR([Oracle Instant Client libraries not found])
  fi
  AC_MSG_RESULT([$OCI8_VERSION])
])

PHP_ARG_WITH(oci8, for Oracle (OCI8) support using ORACLE_HOME installation,
[  --with-oci8[=DIR]       Include Oracle (OCI8) support using an ORACLE_HOME
                          install. The default DIR is ORACLE_HOME])

if test "$PHP_OCI8" = "no"; then
  PHP_ARG_WITH(oci8-instant-client, for Oracle (OCI8) support using Oracle Instant Client,
  [  --with-oci8-instant-client[=DIR]    
                          Include Oracle (OCI8) support using
                          Oracle Instant Client. DIR is the directory with the
                          Instant Client libraries. On Linux it will default to
                          /usr/lib/oracle/<most_recent_version>/client/lib
                          Other platforms will need to have it explicitly specified.])
else 
  PHP_OCI8_INSTANT_CLIENT="no";
fi

if test "$PHP_OCI8" != "no"; then

  if test "$PHP_OCI8_INSTANT_CLIENT" != "no"; then
    AC_MSG_ERROR([--with-oci8 and --with-oci8-instant-client are mutually exclusive])
  fi

  AC_MSG_CHECKING([Oracle Install Directory])
  if test "$PHP_OCI8" = "yes"; then
    OCI8_DIR=$ORACLE_HOME
  else
    OCI8_DIR=$PHP_OCI8
  fi
  AC_MSG_RESULT($OCI8_DIR)

  if test -d "$OCI8_DIR/rdbms/public"; then
    PHP_ADD_INCLUDE($OCI8_DIR/rdbms/public)
    OCI8_INCLUDES="$OCI8_INCLUDES -I$OCI8_DIR/rdbms/public"
  fi
  if test -d "$OCI8_DIR/rdbms/demo"; then
    PHP_ADD_INCLUDE($OCI8_DIR/rdbms/demo)
    OCI8_INCLUDES="$OCI8_INCLUDES -I$OCI8_DIR/rdbms/demo"
  fi
  if test -d "$OCI8_DIR/network/public"; then
    PHP_ADD_INCLUDE($OCI8_DIR/network/public)
    OCI8_INCLUDES="$OCI8_INCLUDES -I$OCI8_DIR/network/public"
  fi
  if test -d "$OCI8_DIR/plsql/public"; then
    PHP_ADD_INCLUDE($OCI8_DIR/plsql/public)
    OCI8_INCLUDES="$OCI8_INCLUDES -I$OCI8_DIR/plsql/public"
  fi

  if test -f "$OCI8_DIR/lib/sysliblist"; then
    PHP_EVAL_LIBLINE(`cat $OCI8_DIR/lib/sysliblist`, OCI8_SYSLIB)
  elif test -f "$OCI8_DIR/rdbms/lib/sysliblist"; then
    PHP_EVAL_LIBLINE(`cat $OCI8_DIR/rdbms/lib/sysliblist`, OCI8_SYSLIB)
  fi

  AC_OCI8_VERSION($OCI8_DIR)
  case $OCI8_VERSION in
    8.0)
      PHP_ADD_LIBRARY_WITH_PATH(nlsrtl3, "", OCI8_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(core4, "", OCI8_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(psa, "", OCI8_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(clntsh, $OCI8_DIR/lib, OCI8_SHARED_LIBADD)
      ;;

    8.1)
      PHP_ADD_LIBRARY(clntsh, 1, OCI8_SHARED_LIBADD)
      PHP_ADD_LIBPATH($OCI8_DIR/lib, OCI8_SHARED_LIBADD)

      dnl 
      dnl OCI_ATTR_STATEMENT is not available in all 8.1.x versions
      dnl 
      PHP_OCI_IF_DEFINED(OCI_ATTR_STATEMENT, [AC_DEFINE(HAVE_OCI8_ATTR_STATEMENT,1,[ ])], $OCI8_INCLUDES)
      ;;

    9.0)
      PHP_ADD_LIBRARY(clntsh, 1, OCI8_SHARED_LIBADD)
      PHP_ADD_LIBPATH($OCI8_DIR/lib, OCI8_SHARED_LIBADD)
      AC_DEFINE(HAVE_OCI8_ATTR_STATEMENT,1,[ ])

      dnl These functions are only available in version >= 9.2
      PHP_CHECK_LIBRARY(clntsh, OCIEnvNlsCreate,
      [
        PHP_CHECK_LIBRARY(clntsh, OCINlsCharSetNameToId,
        [
          AC_DEFINE(HAVE_OCI_9_2,1,[ ])
          OCI8_VERSION=9.2
        ], [], [
          -L$OCI8_DIR/lib $OCI8_SHARED_LIBADD
        ])
      ], [], [
        -L$OCI8_DIR/lib $OCI8_SHARED_LIBADD
      ])
      ;;

    10.1)
      PHP_ADD_LIBRARY(clntsh, 1, OCI8_SHARED_LIBADD)
      PHP_ADD_LIBPATH($OCI8_DIR/lib, OCI8_SHARED_LIBADD)
      AC_DEFINE(HAVE_OCI8_ATTR_STATEMENT,1,[ ])
      AC_DEFINE(HAVE_OCI_9_2,1,[ ])
      AC_DEFINE(HAVE_OCI8_TEMP_LOB,1,[ ])
      AC_DEFINE(PHP_OCI8_HAVE_COLLECTIONS,1,[ ])
      ;;

    *)
      AC_MSG_ERROR([Unsupported Oracle version])
      ;;
  esac

  dnl
  dnl Check if we need to add -locijdbc8 
  dnl
  PHP_CHECK_LIBRARY(clntsh, OCILobIsTemporary,
  [
    AC_DEFINE(HAVE_OCI8_TEMP_LOB,1,[ ])
  ], [
    PHP_CHECK_LIBRARY(ocijdbc8, OCILobIsTemporary,
    [
      PHP_ADD_LIBRARY(ocijdbc8, 1, OCI8_SHARED_LIBADD)
      AC_DEFINE(HAVE_OCI8_TEMP_LOB,1,[ ])
    ], [], [
      -L$OCI8_DIR/lib $OCI8_SHARED_LIBADD
    ])
  ], [
    -L$OCI8_DIR/lib $OCI8_SHARED_LIBADD
  ])

  dnl
  dnl Check if we have collections
  dnl
  PHP_CHECK_LIBRARY(clntsh, OCICollAssign,
  [
    AC_DEFINE(PHP_OCI8_HAVE_COLLECTIONS,1,[ ])
  ], [], [
    -L$OCI8_DIR/lib $OCI8_SHARED_LIBADD
  ])


  PHP_NEW_EXTENSION(oci8, oci8.c, $ext_shared)
  AC_DEFINE(HAVE_OCI8,1,[ ])

  PHP_SUBST_OLD(OCI8_SHARED_LIBADD)
  PHP_SUBST_OLD(OCI8_DIR)
  PHP_SUBST_OLD(OCI8_VERSION)

elif test "$PHP_OCI8_INSTANT_CLIENT" != "no"; then

  AC_MSG_CHECKING([Oracle Instant Client directory])
  if test "$PHP_OCI8_INSTANT_CLIENT" = "yes"; then
dnl Generally the Instant Client can be anywhere so the user must pass in the
dnl directory to the libraries.  But on Linux we default to the most recent
dnl version in /usr/lib
    PHP_OCI8_INSTANT_CLIENT=`ls -d /usr/lib/oracle/*/client/lib  2> /dev/null | tail -1`
    if test -z "$PHP_OCI8_INSTANT_CLIENT"; then
      AC_MSG_ERROR([Oracle Instant Client directory not found. Try --with-oci8-instant-client=DIR])
    fi
  fi
  AC_MSG_RESULT($PHP_OCI8_INSTANT_CLIENT)

  OCI8_DIR=$PHP_OCI8_INSTANT_CLIENT

  AC_MSG_CHECKING([Oracle Instant Client SDK header directory])

dnl Header directory for Instant Client SDK RPM install
  OCISDKRPMINC=`echo "$PHP_OCI8_INSTANT_CLIENT" | sed -e 's!^/usr/lib/oracle/\(.*\)/client/lib[[/]]*$!/usr/include/oracle/\1/client!'`

dnl Header directory for Instant Client SDK zip file install
  OCISDKZIPINC=$PHP_OCI8_INSTANT_CLIENT/sdk/include

  if test -f "$OCISDKRPMINC/oci.h"; then
    AC_MSG_RESULT($OCISDKRPMINC)
    PHP_ADD_INCLUDE($OCISDKRPMINC)
    OCI8INCDIR=$OCISDKRPMINC
  elif test -f "$OCISDKZIPINC/oci.h"; then
    AC_MSG_RESULT($OCISDKZIPINC)
    PHP_ADD_INCLUDE($OCISDKZIPINC)
    OCI8INCDIR=$OCISDKZIPINC
  else
    AC_MSG_ERROR([Oracle Instant Client SDK header files not found])
  fi

  OCISYSLIBLIST=`echo "$OCI8INCDIR" | sed -e 's!\(.*\)/include$!\1/demo/sysliblist!'`
  if test -f "$OCISYSLIBLIST"; then
    PHP_EVAL_LIBLINE(`cat $OCISYSLIBLIST`, OCI8_SYSLIB)
  fi

  AC_OCI8IC_VERSION($PHP_OCI8_INSTANT_CLIENT)
  case $OCI8_VERSION in
    10.1)
      PHP_ADD_LIBRARY(clntsh, 1, OCI8_SHARED_LIBADD)
      PHP_ADD_LIBPATH($PHP_OCI8_INSTANT_CLIENT, OCI8_SHARED_LIBADD)
      ;;

    *)
      AC_MSG_ERROR([Unsupported Oracle Instant Client version])
      ;;
  esac

  AC_DEFINE(HAVE_OCI8_ATTR_STATEMENT,1,[ ])
  AC_DEFINE(HAVE_OCI_9_2,1,[ ])
  AC_DEFINE(HAVE_OCI8_TEMP_LOB,1,[ ])
  AC_DEFINE(PHP_OCI8_HAVE_COLLECTIONS,1,[ ])
  AC_DEFINE(HAVE_OCI_INSTANT_CLIENT,1,[ ])

  PHP_NEW_EXTENSION(oci8, oci8.c, $ext_shared)
  AC_DEFINE(HAVE_OCI8,1,[ ])

  PHP_SUBST_OLD(OCI8_SHARED_LIBADD)
  PHP_SUBST_OLD(OCI8_DIR)
  PHP_SUBST_OLD(OCI8_VERSION)

fi
